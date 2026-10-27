#include "bsp/esp-bsp.h"
#include "bsp/touch.h"
#include "bsp_board_internal.h"

#include "esp_check.h"
#include "esp_log.h"
#include "esp_codec_dev_defaults.h"
#include "esp_lcd_co5300.h"
#include "esp_lcd_touch_cst820.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static const char *TAG = "bsp_1_43c";
static i2c_master_bus_handle_t i2c_bus;
static esp_lcd_panel_io_handle_t lcd_io, touch_io;
static esp_lcd_panel_handle_t panel;
static esp_lcd_touch_handle_t touch;
static lv_display_t *display;
static lv_indev_t *input;
static bool backlight_ready, display_started, port_initialized;
static bool backlight_released;
static int brightness = CONFIG_BSP_LCD_BRIGHTNESS;

/**************************************************************************************************
 * I2C interface
 **************************************************************************************************/
esp_err_t bsp_i2c_init(void)
{
    if (i2c_bus) return ESP_OK;
    i2c_master_bus_config_t cfg = {};
    cfg.i2c_port = BSP_I2C_NUM;
    cfg.sda_io_num = BSP_I2C_SDA;
    cfg.scl_io_num = BSP_I2C_SCL;
    cfg.clk_source = I2C_CLK_SRC_DEFAULT;
    cfg.glitch_ignore_cnt = 7;
    cfg.flags.enable_internal_pullup = true;
    return i2c_new_master_bus(&cfg, &i2c_bus);
}

esp_err_t bsp_i2c_deinit(void)
{
    if (!i2c_bus) return ESP_OK;
    // Child devices must be removed before deleting their common bus.
    if (touch || bsp_board_i2c_in_use()) return ESP_ERR_INVALID_STATE;
    esp_err_t ret = i2c_del_master_bus(i2c_bus);
    if (ret == ESP_OK) i2c_bus = nullptr;
    return ret;
}

i2c_master_bus_handle_t bsp_i2c_get_handle(void)
{
    return bsp_i2c_init() == ESP_OK ? i2c_bus : nullptr;
}

/**************************************************************************************************
 * I2S audio interface: ES8311 playback and ES7210 microphone capture.
 **************************************************************************************************/
static i2s_chan_handle_t tx, rx;
static const audio_codec_data_if_t *data_if;
static const audio_codec_ctrl_if_t *ctrl_if;
static const audio_codec_gpio_if_t *gpio_if;
static const audio_codec_if_t *codec_if;
static esp_codec_dev_handle_t codec;
struct MicrophoneCodec {
    const audio_codec_ctrl_if_t *ctrl_if;
    const audio_codec_if_t *codec_if;
    esp_codec_dev_handle_t codec;
};
static MicrophoneCodec microphones[2];

bool bsp_board_audio_in_use(void) { return data_if || codec; }

static void bsp_audio_cleanup(void)
{
    for (auto &mic : microphones) {
        if (mic.codec) esp_codec_dev_delete(mic.codec);
        if (mic.codec_if) audio_codec_delete_codec_if(mic.codec_if);
        if (mic.ctrl_if) audio_codec_delete_ctrl_if(mic.ctrl_if);
        mic = {};
    }
    if (codec) { esp_codec_dev_delete(codec); codec = nullptr; }
    if (codec_if) { audio_codec_delete_codec_if(codec_if); codec_if = nullptr; }
    if (ctrl_if) { audio_codec_delete_ctrl_if(ctrl_if); ctrl_if = nullptr; }
    if (gpio_if) { audio_codec_delete_gpio_if(gpio_if); gpio_if = nullptr; }
    if (data_if) { audio_codec_delete_data_if(data_if); data_if = nullptr; }
    if (tx) { i2s_channel_disable(tx); i2s_del_channel(tx); tx = nullptr; }
    if (rx) { i2s_channel_disable(rx); i2s_del_channel(rx); rx = nullptr; }
}

esp_err_t bsp_audio_deinit(void)
{
    // All APP workers must be stopped before releasing these BSP-owned handles.
    bsp_audio_cleanup();
    return ESP_OK;
}

static esp_err_t bsp_audio_init_channels(const i2s_std_config_t *i2s_config)
{
    i2s_chan_config_t channel = I2S_CHANNEL_DEFAULT_CONFIG(BSP_I2S_NUM, I2S_ROLE_MASTER);
    channel.dma_desc_num = BSP_AUDIO_DMA_DESCRIPTORS;
    channel.dma_frame_num = BSP_AUDIO_DMA_FRAMES;
    channel.auto_clear_after_cb = true;
    ESP_RETURN_ON_ERROR(i2s_new_channel(&channel, &tx, &rx), TAG, "I2S channels");
    i2s_std_config_t cfg = {};
    if (i2s_config) cfg = *i2s_config;
    else {
        cfg.clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(22050);
        cfg.slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO);
        cfg.gpio_cfg.mclk = BSP_I2S_MCLK;
        cfg.gpio_cfg.bclk = BSP_I2S_BCLK;
        cfg.gpio_cfg.ws = BSP_I2S_LRCLK;
        cfg.gpio_cfg.dout = BSP_I2S_DOUT;
        cfg.gpio_cfg.din = BSP_I2S_DIN;
    }
    ESP_RETURN_ON_ERROR(i2s_channel_init_std_mode(tx, &cfg), TAG, "I2S TX");
    ESP_RETURN_ON_ERROR(i2s_channel_init_std_mode(rx, &cfg), TAG, "I2S RX");
    ESP_RETURN_ON_ERROR(i2s_channel_enable(tx), TAG, "I2S TX enable");
    ESP_RETURN_ON_ERROR(i2s_channel_enable(rx), TAG, "I2S RX enable");
    audio_codec_i2s_cfg_t data = {};
    data.port = BSP_I2S_NUM;
    data.tx_handle = tx;
    data.rx_handle = rx;
    data_if = audio_codec_new_i2s_data(&data);
    return data_if ? ESP_OK : ESP_ERR_NO_MEM;
}

esp_err_t bsp_audio_init(const i2s_std_config_t *i2s_config)
{
    if (data_if) return ESP_OK;
    ESP_RETURN_ON_ERROR(bsp_i2c_init(), TAG, "I2C bus");
    // The ES8311 and ES7210 share an active-high audio LDO.
    ESP_RETURN_ON_ERROR(gpio_set_level(BSP_AUDIO_LDO_EN, 1), TAG, "Audio LDO level");
    ESP_RETURN_ON_ERROR(gpio_set_direction(BSP_AUDIO_LDO_EN, GPIO_MODE_OUTPUT), TAG, "Audio LDO enable");
    vTaskDelay(pdMS_TO_TICKS(10));
    esp_err_t ret = bsp_audio_init_channels(i2s_config);
    if (ret != ESP_OK) bsp_audio_cleanup();
    return ret;
}

esp_err_t bsp_audio_init_voice_24k(void)
{
    i2s_std_config_t cfg = {};
    cfg.clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(24000);
    cfg.slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
    cfg.gpio_cfg.mclk = BSP_I2S_MCLK;
    cfg.gpio_cfg.bclk = BSP_I2S_BCLK;
    cfg.gpio_cfg.ws = BSP_I2S_LRCLK;
    cfg.gpio_cfg.dout = BSP_I2S_DOUT;
    cfg.gpio_cfg.din = BSP_I2S_DIN;
    return bsp_audio_init(&cfg);
}

esp_codec_dev_handle_t bsp_audio_codec_speaker_init(void)
{
    if (codec) return codec;
    if (bsp_audio_init(nullptr) != ESP_OK) return nullptr;
    // Codec control uses an 8-bit address; the bus probe takes 7 bits.
    if (i2c_master_probe(bsp_i2c_get_handle(), ES8311_CODEC_DEFAULT_ADDR >> 1, 100) != ESP_OK) return nullptr;
    audio_codec_i2c_cfg_t ctrl = {};
    ctrl.port = BSP_I2C_NUM;
    ctrl.addr = ES8311_CODEC_DEFAULT_ADDR;
    ctrl.bus_handle = bsp_i2c_get_handle();
    ctrl_if = audio_codec_new_i2c_ctrl(&ctrl);
    gpio_if = audio_codec_new_gpio();
    if (!ctrl_if || !gpio_if) goto failed;
    {
        es8311_codec_cfg_t chip = {};
        chip.codec_mode = ESP_CODEC_DEV_WORK_MODE_DAC;
        chip.ctrl_if = ctrl_if;
        chip.gpio_if = gpio_if;
        chip.pa_pin = BSP_AUDIO_PA;
        chip.use_mclk = true;
        chip.master_mode = false;
        chip.hw_gain.pa_voltage = 5.0f;
        chip.hw_gain.codec_dac_voltage = 3.3f;
        codec_if = es8311_codec_new(&chip);
        if (!codec_if) goto failed;
        esp_codec_dev_cfg_t dev = {};
        dev.dev_type = ESP_CODEC_DEV_TYPE_OUT;
        dev.codec_if = codec_if;
        dev.data_if = data_if;
        codec = esp_codec_dev_new(&dev);
        if (!codec) goto failed;
    }
    return codec;
failed:
    bsp_audio_cleanup();
    return nullptr;
}

static esp_codec_dev_handle_t bsp_audio_codec_microphone_new(unsigned index, uint8_t mic_selected)
{
    auto &mic = microphones[index];
    if (mic.codec) return mic.codec;
    if (bsp_audio_init(nullptr) != ESP_OK) return nullptr;
    audio_codec_i2c_cfg_t ctrl = {};
    ctrl.port = BSP_I2C_NUM;
    ctrl.addr = ES7210_CODEC_DEFAULT_ADDR;
    ctrl.bus_handle = bsp_i2c_get_handle();
    mic.ctrl_if = audio_codec_new_i2c_ctrl(&ctrl);
    if (!mic.ctrl_if) return nullptr;
    es7210_codec_cfg_t chip = {};
    chip.ctrl_if = mic.ctrl_if;
    chip.mic_selected = mic_selected;
    mic.codec_if = es7210_codec_new(&chip);
    if (mic.codec_if) {
        esp_codec_dev_cfg_t dev = {};
        dev.dev_type = ESP_CODEC_DEV_TYPE_IN;
        dev.codec_if = mic.codec_if;
        dev.data_if = data_if;
        mic.codec = esp_codec_dev_new(&dev);
    }
    if (!mic.codec) {
        if (mic.codec_if) audio_codec_delete_codec_if(mic.codec_if);
        audio_codec_delete_ctrl_if(mic.ctrl_if);
        mic = {};
    }
    return mic.codec;
}

esp_codec_dev_handle_t bsp_audio_codec_microphone_init(void)
{
    return bsp_audio_codec_microphone_new(0, 0);
}

esp_codec_dev_handle_t bsp_audio_codec_voice_microphone_init(void)
{
    // MIC3 carries the DAC reference. Three enabled inputs select four-slot TDM.
    return bsp_audio_codec_microphone_new(1, ES7210_SEL_MIC1 | ES7210_SEL_MIC2 | ES7210_SEL_MIC3);
}

esp_err_t bsp_audio_i2s_read(void *data, size_t size, size_t *read, uint32_t timeout_ms)
{
    return rx ? i2s_channel_read(rx, data, size, read, timeout_ms) : ESP_ERR_INVALID_STATE;
}
esp_err_t bsp_audio_i2s_write(void *data, size_t size, size_t *written, uint32_t timeout_ms)
{
    return tx ? i2s_channel_write(tx, data, size, written, timeout_ms) : ESP_ERR_INVALID_STATE;
}

esp_err_t bsp_display_brightness_init(void)
{
    // CO5300 has no separate backlight. Its initialization table keeps 0x51 at zero.
    backlight_ready = true;
    return ESP_OK;
}

esp_err_t bsp_display_brightness_set(int percent)
{
    if (percent < 0 || percent > 100) return ESP_ERR_INVALID_ARG;
    if (!backlight_ready) return ESP_ERR_INVALID_STATE;
    if (panel) {
        ESP_RETURN_ON_ERROR(esp_lcd_panel_co5300_set_brightness(panel, backlight_released ? percent : 0),
                            TAG, "AMOLED brightness");
    }
    brightness = percent;
    return ESP_OK;
}

int bsp_display_brightness_get(void) { return brightness; }

esp_err_t bsp_display_backlight_off(void) { return bsp_display_brightness_set(0); }

esp_err_t bsp_display_backlight_on(void) { return bsp_display_brightness_set(CONFIG_BSP_LCD_BRIGHTNESS); }

esp_err_t bsp_display_show(void)
{
    if (!display_started || !display || !lcd_io) return ESP_ERR_INVALID_STATE;
    if (!bsp_display_lock(0)) return ESP_ERR_TIMEOUT;
    lv_obj_invalidate(lv_display_get_screen_active(display));
    lv_refr_now(display);
    // Drain the final QSPI DMA transfer before releasing the initial backlight hold.
    esp_err_t ret = esp_lcd_panel_io_tx_param(lcd_io, -1, nullptr, 0);
    if (ret == ESP_OK) {
        backlight_released = true;
        ret = bsp_display_brightness_set(brightness);
        if (ret != ESP_OK) backlight_released = false;
    }
    bsp_display_unlock();
    if (ret == ESP_OK) ESP_LOGI(TAG, "Initial UI frame flushed; backlight enabled at %d%%", brightness);
    return ret;
}

// CO5300 module initialization. Keep it dark until the first desktop frame.
static const uint8_t cmd_zero[] = {0x00}, cmd_c4[] = {0x80};
static const uint8_t cmd_control[] = {0x20}, cmd_hbm[] = {0xff};
static const co5300_lcd_init_cmd_t lcd_init_cmds[] = {
    {0xfe, cmd_zero, 1, 0}, {0xc4, cmd_c4, 1, 0},
    {0x35, cmd_zero, 1, 10}, {0x53, cmd_control, 1, 10},
    {0x51, cmd_zero, 1, 10}, {0x63, cmd_hbm, 1, 10},
    {0x11, nullptr, 0, 120},
};

esp_err_t bsp_display_new(const bsp_display_config_t *config, esp_lcd_panel_handle_t *ret_panel,
                          esp_lcd_panel_io_handle_t *ret_io)
{
    if (!ret_panel || !ret_io) return ESP_ERR_INVALID_ARG;
    *ret_panel = nullptr; *ret_io = nullptr;
    esp_lcd_panel_io_handle_t new_io = nullptr;
    esp_lcd_panel_handle_t new_panel = nullptr;
    bool bus_created = false;
    auto create = [&]() -> esp_err_t {
        ESP_RETURN_ON_ERROR(bsp_display_brightness_init(), TAG, "Brightness");
        spi_bus_config_t bus = {};
        bus.sclk_io_num = BSP_LCD_SCLK;
        bus.data0_io_num = BSP_LCD_DATA0; bus.data1_io_num = BSP_LCD_DATA1;
        bus.data2_io_num = BSP_LCD_DATA2; bus.data3_io_num = BSP_LCD_DATA3;
        bus.max_transfer_sz = config && config->max_transfer_sz ? config->max_transfer_sz : BSP_LCD_H_RES * BSP_LCD_V_RES * 2;
        bus.flags = SPICOMMON_BUSFLAG_QUAD;
        ESP_RETURN_ON_ERROR(spi_bus_initialize(BSP_LCD_SPI_HOST, &bus, SPI_DMA_CH_AUTO), TAG, "AMOLED QSPI");
        bus_created = true;
        esp_lcd_panel_io_spi_config_t io = {};
        io.cs_gpio_num = BSP_LCD_CS; io.dc_gpio_num = -1;
        io.spi_mode = 0; io.pclk_hz = CONFIG_BSP_LCD_SPI_CLOCK_HZ;
        io.trans_queue_depth = 2; io.lcd_cmd_bits = 32; io.lcd_param_bits = 8;
        io.flags.quad_mode = true;
        ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)BSP_LCD_SPI_HOST, &io, &new_io), TAG, "AMOLED IO");
        co5300_vendor_config_t vendor = {};
        vendor.flags.use_qspi_interface = true;
        vendor.init_cmds = lcd_init_cmds;
        vendor.init_cmds_size = sizeof(lcd_init_cmds) / sizeof(lcd_init_cmds[0]);
        esp_lcd_panel_dev_config_t cfg = {};
        cfg.reset_gpio_num = BSP_LCD_RST; cfg.rgb_ele_order = BSP_LCD_COLOR_SPACE;
        cfg.bits_per_pixel = BSP_LCD_BITS_PER_PIXEL; cfg.vendor_config = &vendor;
        ESP_RETURN_ON_ERROR(esp_lcd_new_panel_co5300(new_io, &cfg, &new_panel), TAG, "CO5300 create");
        ESP_RETURN_ON_ERROR(esp_lcd_panel_reset(new_panel), TAG, "AMOLED reset");
        ESP_RETURN_ON_ERROR(esp_lcd_panel_init(new_panel), TAG, "AMOLED init");
        ESP_RETURN_ON_ERROR(esp_lcd_panel_mirror(new_panel, true, true), TAG, "Panel orientation");
        return esp_lcd_panel_set_gap(new_panel, 8, 0);
    };
    esp_err_t ret = create();
    if (ret != ESP_OK) {
        if (new_panel) esp_lcd_panel_del(new_panel);
        if (new_io) esp_lcd_panel_io_del(new_io);
        if (bus_created) spi_bus_free(BSP_LCD_SPI_HOST);
        return ret;
    }
    *ret_panel = new_panel; *ret_io = new_io;
    return ESP_OK;
}

static void touch_coordinates(esp_lcd_touch_handle_t, uint16_t *x, uint16_t *y,
                              uint16_t *, uint8_t *count, uint8_t)
{
    // Match the original panel's 180-degree orientation, within 0..465.
    for (unsigned i = 0; i < *count; ++i) {
        if (x[i] >= BSP_LCD_H_RES || y[i] >= BSP_LCD_V_RES) { *count = 0; return; }
        x[i] = BSP_LCD_H_RES - 1 - x[i];
        y[i] = BSP_LCD_V_RES - 1 - y[i];
    }
}

esp_err_t bsp_touch_new(const bsp_touch_config_t *config, esp_lcd_touch_handle_t *ret_touch)
{
    (void)config;
    if (!ret_touch) return ESP_ERR_INVALID_ARG;
    if (touch || touch_io) return ESP_ERR_INVALID_STATE;
    *ret_touch = nullptr;
    ESP_RETURN_ON_ERROR(bsp_i2c_init(), TAG, "I2C");
    esp_lcd_panel_io_i2c_config_t io = {};
    io.dev_addr = 0x15;
    io.control_phase_bytes = 1;
    io.lcd_cmd_bits = 8;
    io.lcd_param_bits = 0;
    io.flags.disable_control_phase = 1;
    io.scl_speed_hz = BSP_I2C_FREQ_HZ;
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_i2c(i2c_bus, &io, &touch_io), TAG, "Touch IO");
    esp_lcd_touch_config_t cfg = {};
    cfg.x_max = BSP_LCD_H_RES;
    cfg.y_max = BSP_LCD_V_RES;
    cfg.process_coordinates = touch_coordinates;
    cfg.rst_gpio_num = BSP_TOUCH_RST;
    cfg.int_gpio_num = BSP_TOUCH_INT;
    esp_err_t ret = bsp_touch_new_cst820(touch_io, &cfg, &touch);
    if (ret != ESP_OK) {
        touch = nullptr;
        esp_lcd_panel_io_del(touch_io);
        touch_io = nullptr;
        return ret;
    }
    *ret_touch = touch;
    return ESP_OK;
}

static void display_round_area(lv_area_t *area, void *)
{
    // CO5300 requires two-pixel address alignment, including partial refreshes.
    area->x1 &= ~1; area->y1 &= ~1;
    area->x2 |= 1; area->y2 |= 1;
    if (area->x2 >= BSP_LCD_H_RES) area->x2 = BSP_LCD_H_RES - 1;
    if (area->y2 >= BSP_LCD_V_RES) area->y2 = BSP_LCD_V_RES - 1;
}

static lv_display_t *bsp_display_lcd_init(const bsp_display_cfg_t *cfg)
{
    bsp_display_config_t config = {};
    if (bsp_display_new(&config, &panel, &lcd_io) != ESP_OK) return nullptr;
    esp_lv_adapter_display_config_t disp = {};
    disp.panel = panel;
    disp.panel_io = lcd_io;
    disp.profile.interface = ESP_LV_ADAPTER_PANEL_IF_OTHER;
    disp.profile.rotation = ESP_LV_ADAPTER_ROTATE_0;
    disp.profile.hor_res = BSP_LCD_H_RES;
    disp.profile.ver_res = BSP_LCD_V_RES;
    disp.profile.buffer_height = cfg->buffer_size / BSP_LCD_H_RES;
    disp.profile.use_psram = cfg->flags.buff_spiram;
    disp.profile.require_double_buffer = cfg->double_buffer;
    disp.tear_avoid_mode = ESP_LV_ADAPTER_TEAR_AVOID_MODE_NONE;
    ESP_LOGI(TAG, "Display buffers: %u lines, internal RAM; no PSRAM-to-DMA bounce per full strip",
             unsigned(disp.profile.buffer_height));
    lv_display_t *result = esp_lv_adapter_register_display(&disp);
    if (result && esp_lv_adapter_set_area_rounder_cb(result, display_round_area, nullptr) != ESP_OK) {
        esp_lv_adapter_unregister_display(result);
        return nullptr;
    }
    return result;
}

static lv_indev_t *bsp_display_indev_init(lv_display_t *disp)
{
    bsp_board_status.touch = bsp_touch_new(nullptr, &touch);
    if (bsp_board_status.touch != ESP_OK) return nullptr;
    const esp_lv_adapter_touch_config_t cfg = ESP_LV_ADAPTER_TOUCH_DEFAULT_CONFIG(disp, touch);
    return esp_lv_adapter_register_touch(&cfg);
}

lv_display_t *bsp_display_start(void)
{
    bsp_display_cfg_t cfg = {};
    cfg.lv_adapter_cfg = ESP_LV_ADAPTER_DEFAULT_CONFIG();
    // Desktop rendering and APP callbacks share this task's stack.
    cfg.lv_adapter_cfg.task_stack_size = CONFIG_BSP_LVGL_TASK_STACK_SIZE;
    cfg.lv_adapter_cfg.stack_in_psram = false;
    cfg.buffer_size = BSP_LCD_H_RES * CONFIG_BSP_LCD_BUFFER_HEIGHT;
    cfg.double_buffer = true;
    cfg.flags.buff_spiram = false;
    return bsp_display_start_with_config(&cfg);
}

lv_display_t *bsp_display_start_with_config(const bsp_display_cfg_t *cfg)
{
    if (display_started) return display;
    if (!cfg || !cfg->buffer_size || cfg->buffer_size % BSP_LCD_H_RES ||
        cfg->buffer_size > BSP_LCD_H_RES * BSP_LCD_V_RES) return nullptr;
    bsp_board_status.display = bsp_display_brightness_init();
    if (bsp_board_status.display != ESP_OK) return nullptr;
    bsp_board_status.i2c = bsp_i2c_init();
    if (bsp_board_status.i2c != ESP_OK) return nullptr;
    ESP_LOGI(TAG, "LVGL task stack: %u bytes, %s", unsigned(cfg->lv_adapter_cfg.task_stack_size),
             cfg->lv_adapter_cfg.stack_in_psram ? "PSRAM" : "internal RAM");
    bsp_board_status.display = esp_lv_adapter_init(&cfg->lv_adapter_cfg);
    if (bsp_board_status.display != ESP_OK) return nullptr;
    port_initialized = true;
    display = bsp_display_lcd_init(cfg);
    if (!display) { bsp_board_status.display = ESP_FAIL; goto failed; }
    {
        if (!bsp_display_lock(0)) { bsp_board_status.display = ESP_FAIL; goto failed; }
        input = bsp_display_indev_init(display);
        if (!input) {
            if (bsp_board_status.touch == ESP_OK) bsp_board_status.touch = ESP_ERR_NO_MEM;
            ESP_LOGW(TAG, "Touch: %s", esp_err_to_name(bsp_board_status.touch));
            if (touch) { esp_lcd_touch_del(touch); touch = nullptr; }
            if (touch_io) { esp_lcd_panel_io_del(touch_io); touch_io = nullptr; }
        }
        bsp_display_unlock();
    }
    bsp_board_status.display = esp_lcd_panel_disp_on_off(panel, true);
    if (bsp_board_status.display != ESP_OK) goto failed;
    bsp_board_status.display = esp_lv_adapter_start();
    if (bsp_board_status.display != ESP_OK) goto failed;
    display_started = true;
    ESP_LOGI(TAG, "CO5300 %dx%d RGB565; LVGL %d.%d.%d, esp_lvgl_adapter, two draw buffers",
             BSP_LCD_H_RES, BSP_LCD_V_RES, LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);
    return display;
failed:
    ESP_LOGE(TAG, "Display startup failed: %s", esp_err_to_name(bsp_board_status.display));
    bsp_display_backlight_off();
    backlight_released = false;
    if (input) { esp_lv_adapter_unregister_touch(input); input = nullptr; }
    if (display) { esp_lv_adapter_unregister_display(display); display = nullptr; }
    if (port_initialized) { esp_lv_adapter_deinit(); port_initialized = false; }
    if (touch) { esp_lcd_touch_del(touch); touch = nullptr; }
    if (touch_io) { esp_lcd_panel_io_del(touch_io); touch_io = nullptr; }
    if (panel) { esp_lcd_panel_del(panel); panel = nullptr; }
    if (lcd_io) { esp_lcd_panel_io_del(lcd_io); lcd_io = nullptr; }
    spi_bus_free(BSP_LCD_SPI_HOST);
    return nullptr;
}

lv_display_t *bsp_display_get(void) { return display; }
lv_indev_t *bsp_display_get_input_dev(void) { return input; }
esp_lcd_touch_handle_t bsp_touch_get_handle(void) { return touch; }
void bsp_display_rotate(lv_display_t *disp, lv_disp_rotation_t rotation)
{
    lv_display_set_rotation(disp, rotation);
}
bool bsp_display_lock(uint32_t timeout_ms)
{
    return esp_lv_adapter_lock(timeout_ms == 0 || timeout_ms == UINT32_MAX ? -1 : (int32_t)timeout_ms) == ESP_OK;
}
void bsp_display_unlock(void) { esp_lv_adapter_unlock(); }
