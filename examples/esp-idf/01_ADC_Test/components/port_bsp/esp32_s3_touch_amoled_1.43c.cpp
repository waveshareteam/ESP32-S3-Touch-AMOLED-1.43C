#include "esp32_s3_touch_amoled_1.43c.h"
#include "esp_lcd_sh8601.h"
#include "bsp_config.h"
#include "i2c_bsp.h"
#include "lvgl.h"

#include <esp_adc/adc_oneshot.h>
#include <driver/gpio.h>
#include <esp_log.h>
#include <driver/ledc.h>
#include <freertos/FreeRTOS.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_vendor.h>
#include <esp_lcd_panel_ops.h>
#include <esp_spiffs.h>
#include <esp_vfs_fat.h>

#define TAG "DISP_BSP"

static esp_lcd_panel_io_handle_t io_handle = NULL;
static esp_lcd_panel_handle_t panel_handle = NULL;
static uint8_t brightness;
static I2cMasterBus *i2c_bus = nullptr;
static i2c_master_dev_handle_t touch_dev_handle = NULL;
static bsp_lvgl_t result = {0};
static adc_cali_handle_t cali_handle;
static adc_oneshot_unit_handle_t adc1_handle;
static bool i2s_initialized = false;

static const sh8601_lcd_init_cmd_t lcd_init_cmds[] = 
{
    {0xFE, (uint8_t []){0x00}, 1, 0},   
    {0xC4, (uint8_t []){0x80}, 1, 0},
    {0x3A, (uint8_t []){0x55}, 1, 0},
    {0x35, (uint8_t []){0x00}, 1, 0},
    {0x53, (uint8_t []){0x20}, 1, 0},
    {0x51, (uint8_t []){0xFF}, 1, 0}, 
    {0x36, (uint8_t []){0xC0}, 1, 0}, 
    {0x63, (uint8_t []){0xFF}, 1, 0},
    {0x2A, (uint8_t []){0x00,0x06,0x01,0xD7}, 4, 0}, 
    {0x2B, (uint8_t []){0x00,0x00,0x01,0xD1}, 4, 0}, 
    {0x11, (uint8_t []){0x00}, 0, 100}, 
    {0x29, (uint8_t []){0x00}, 0, 0}, 
};

void bsp_lcd_init(void) {
    int ret = ESP_OK;
    spi_bus_config_t buscfg = {};
    buscfg.sclk_io_num = LCD_SCK_PIN;                              
    buscfg.data0_io_num = LCD_D0_PIN;                                     
    buscfg.data1_io_num = LCD_D1_PIN;                                     
    buscfg.data2_io_num = LCD_D2_PIN;                                     
    buscfg.data3_io_num = LCD_D3_PIN;                                                         
    buscfg.max_transfer_sz = (LCD_WIDTH * LCD_HEIGHT * 2);
    ret = spi_bus_initialize(BSP_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);

    esp_lcd_panel_io_spi_config_t io_config = {};
    io_config.cs_gpio_num = LCD_CS_PIN;             
    io_config.dc_gpio_num = -1;             
    io_config.spi_mode = 0;                
    io_config.pclk_hz = 40 * 1000 * 1000;    
    io_config.trans_queue_depth = 2;    
    io_config.on_color_trans_done = NULL;      
    io_config.user_ctx = NULL;             
    io_config.lcd_cmd_bits = 32;             
    io_config.lcd_param_bits = 8;            
    io_config.flags.quad_mode = true;
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(BSP_SPI_HOST, &io_config, &io_handle));

    sh8601_vendor_config_t vendor_config = {};
    vendor_config.init_cmds = lcd_init_cmds;
    vendor_config.init_cmds_size = sizeof(lcd_init_cmds) / sizeof(lcd_init_cmds[0]);
    vendor_config.flags.use_qspi_interface = 1;

    esp_lcd_panel_dev_config_t panel_config = {};
    panel_config.reset_gpio_num = LCD_RST_PIN;
    panel_config.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB;
    panel_config.bits_per_pixel = (16);
    panel_config.vendor_config = &vendor_config;
    ESP_LOGI(TAG, "Install SH8601 panel driver");
    ESP_ERROR_CHECK(esp_lcd_new_panel_sh8601(io_handle, &panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_set_gap(panel_handle, 0x08,0));
}

esp_err_t bsp_display_brightness_set(int brightness_percent)
{
    if (panel_handle == NULL)
    {
        ESP_LOGE(TAG, "Panel handle is not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (brightness_percent < 0 || brightness_percent > 100)
    {
        ESP_LOGE(TAG, "Invalid brightness percentage. Should be between 0 and 100.");
        return ESP_ERR_INVALID_ARG;
    }

    brightness = (uint8_t)(brightness_percent * 255 / 100);

    uint32_t lcd_cmd = 0x51;
    lcd_cmd &= 0xff;
    lcd_cmd <<= 8;
    lcd_cmd |= 0x02 << 24;
    uint8_t param = brightness;
    esp_lcd_panel_io_tx_param(io_handle, lcd_cmd, &param, 1);
    
    return ESP_OK;
}

int bsp_display_brightness_get(void)
{
    if (panel_handle == NULL)
    {
        ESP_LOGE(TAG, "Panel handle is not initialized");
        return -1;
    }

    return brightness * 100 / 255;
}

esp_err_t bsp_display_brightness_init(void)
{
    bsp_display_brightness_set(100);
    return ESP_OK;
}

esp_err_t bsp_display_lock(int32_t timeout_ms) {
    return esp_lv_adapter_lock(timeout_ms);
}

void bsp_display_unlock(void) {
    esp_lv_adapter_unlock();
}

void bsp_touch_init(i2c_master_bus_handle_t BusHandle) {
    i2c_device_config_t     dev_cfg   = {};
    dev_cfg.dev_addr_length           = I2C_ADDR_BIT_LEN_7;
    dev_cfg.scl_speed_hz              = 400000;
    dev_cfg.device_address            = 0x15;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(BusHandle, &dev_cfg, &touch_dev_handle));

    gpio_config_t gpio_conf = {};
    gpio_conf.intr_type     = GPIO_INTR_DISABLE;
    gpio_conf.mode          = GPIO_MODE_OUTPUT;
    gpio_conf.pin_bit_mask  = (0x1ULL<<TP_RST_PIN);
    gpio_conf.pull_down_en  = GPIO_PULLDOWN_DISABLE;
    gpio_conf.pull_up_en    = GPIO_PULLUP_ENABLE;

    ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_config(&gpio_conf));

    gpio_set_level((gpio_num_t)TP_RST_PIN,1);
    vTaskDelay(pdMS_TO_TICKS(200));
    gpio_set_level((gpio_num_t)TP_RST_PIN,0);
    vTaskDelay(pdMS_TO_TICKS(200));
    gpio_set_level((gpio_num_t)TP_RST_PIN,1);
    vTaskDelay(pdMS_TO_TICKS(200));
}

void bsp_batt_init(void) {
    adc_cali_curve_fitting_config_t cali_config = {};
    cali_config.unit_id = ADC_UNIT_1;
    cali_config.atten = ADC_ATTEN_DB_12;
    cali_config.bitwidth = ADC_BITWIDTH_12;
  	ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cali_config, &cali_handle));

	adc_oneshot_unit_init_cfg_t init_config1 = {};
    init_config1.unit_id = ADC_UNIT_1;
  	ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));
  	adc_oneshot_chan_cfg_t config = {};
    config.bitwidth = ADC_BITWIDTH_12;            
    config.atten = ADC_ATTEN_DB_12;
  	ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_3, &config));

	gpio_config_t gpio_conf = {};
  	gpio_conf.intr_type = GPIO_INTR_DISABLE;
  	gpio_conf.mode = GPIO_MODE_INPUT;
  	gpio_conf.pin_bit_mask = (0x1ULL<<GPIO_NUM_7);
  	gpio_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  	gpio_conf.pull_up_en = GPIO_PULLUP_ENABLE;

  	ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_config(&gpio_conf));
}

uint16_t bsp_batt_get_voltage(void) {
    int value;
  	int tage = 0;
    float vol = 0;
  	esp_err_t err;
  	err = adc_oneshot_read(adc1_handle,ADC_CHANNEL_3,&value);
  	if(err == ESP_OK) {
    	adc_cali_raw_to_voltage(cali_handle,value,&tage);
    	vol = tage * 2;
	}
	return vol;
}

/*1表示正在充电，0表示未充电*/
uint8_t bsp_batt_get_status(void) {
    int level = gpio_get_level(GPIO_NUM_7);
    return (level == 0) ? 1 : 0; // 0表示正在充电，1表示未充电
}

#if LVGL_VERSION_MAJOR >= 9

static void rounder_event_cb(lv_event_t *e)
{
    lv_area_t *area = (lv_area_t *)lv_event_get_param(e);
    uint16_t x1 = area->x1;
    uint16_t x2 = area->x2;

    uint16_t y1 = area->y1;
    uint16_t y2 = area->y2;

    // round the start of coordinate down to the nearest 2M number
    area->x1 = (x1 >> 1) << 1;
    area->y1 = (y1 >> 1) << 1;
    // round the end of coordinate up to the nearest 2N+1 number
    area->x2 = ((x2 >> 1) << 1) + 1;
    area->y2 = ((y2 >> 1) << 1) + 1;
}

uint8_t GetCoords(uint16_t *x, uint16_t *y) {
    uint8_t GestureNum[2] = {0, 0};
    uint8_t Event         = 0x01;
    uint8_t Gpos[4]       = {0};
    i2c_bus->i2c_read_buff(touch_dev_handle, 0x02, GestureNum, 2);
    Event = GestureNum[1] >> 6;
    if (GestureNum[0] && (Event != 0x01)) {
        i2c_bus->i2c_read_buff(touch_dev_handle, 0x03, Gpos, 4);
        *x = (((uint16_t) Gpos[0] & 0x0f) << 8 | Gpos[1]);
        *y = (((uint16_t) Gpos[2] & 0x0f) << 8 | Gpos[3]);
        return 1;
    }
    return 0;
}

void my_lvgl_indev_cb(lv_indev_t * indev, lv_indev_data_t *indevData) {
    uint16_t tp_x = 0x00;
    uint16_t tp_y = 0x00;
    if(GetCoords(&tp_x,&tp_y)) {
        indevData->point.x = LCD_WIDTH - tp_x;
        indevData->point.y = LCD_HEIGHT - tp_y;
        if(indevData->point.x > LCD_WIDTH)
        indevData->point.x = LCD_WIDTH;
        if(indevData->point.y > LCD_HEIGHT)
        indevData->point.y = LCD_HEIGHT;
        indevData->state = LV_INDEV_STATE_PRESSED;
        //ESP_LOGW(TAG, "Touch at (%d, %d)", indevData->point.x, indevData->point.y);
    } else {
        indevData->state = LV_INDEV_STATE_RELEASED;
    }
}

#else
static void bsp_lvgl_rounder_cb(lv_disp_drv_t *disp_drv, lv_area_t *area)
{
    uint16_t x1 = area->x1;
    uint16_t x2 = area->x2;

    uint16_t y1 = area->y1;
    uint16_t y2 = area->y2;

    // round the start of coordinate down to the nearest 2M number
    area->x1 = (x1 >> 1) << 1;
    area->y1 = (y1 >> 1) << 1;
    // round the end of coordinate up to the nearest 2N+1 number
    area->x2 = ((x2 >> 1) << 1) + 1;
    area->y2 = ((y2 >> 1) << 1) + 1;
}
#endif

bsp_lvgl_t bsp_broolesia_display_init(void) {
    i2c_bus = I2cMasterBus::requestInstance(ESP32_I2C_SCL,ESP32_I2C_SDA,BSP_I2C_HOST);
    assert(i2c_bus);
    bsp_batt_init();
    bsp_lcd_init();
    /*lvgl*/
    esp_lv_adapter_config_t lv_adapter_cfg = ESP_LV_ADAPTER_DEFAULT_CONFIG();
    lv_adapter_cfg.task_stack_size = 8 * 1024;      // 栈大小改为8KB，  彻底解决栈溢出
    lv_adapter_cfg.task_priority = 8;               // 优先级提升到8， 解决UI卡顿
    lv_adapter_cfg.task_core_id = 1;                // 强制绑定到Core 1，最关键优化！
    lv_adapter_cfg.stack_in_psram = true;           // 可选：栈空间分配到PSRAM(有PSRAM必开)
    ESP_ERROR_CHECK(esp_lv_adapter_init(&lv_adapter_cfg));

    esp_lv_adapter_display_config_t disp_cfg = ESP_LV_ADAPTER_DISPLAY_SPI_WITH_PSRAM_DEFAULT_CONFIG(
        panel_handle,           		// LCD 面板句柄
        io_handle,        				// LCD 面板 IO 句柄（某些接口可为 NULL）
        LCD_WIDTH,             		    // 水平分辨率
        LCD_HEIGHT,             		// 垂直分辨率
        ESP_LV_ADAPTER_ROTATE_0 		// 旋转角度
    );
    disp_cfg.profile.buffer_height = 30; // 设置缓冲区高度，单位为像素行，根据实际情况调整
    lv_display_t *disp = esp_lv_adapter_register_display(&disp_cfg);
    assert(disp != NULL);
    bsp_touch_init(i2c_bus->Get_I2cBusHandle());
#if LVGL_VERSION_MAJOR >= 9
    lv_display_add_event_cb(disp, rounder_event_cb, LV_EVENT_INVALIDATE_AREA, NULL);
    
    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_lvgl_indev_cb);

#else
    lv_disp_t *disp_v8 = (lv_disp_t *)disp;
    if (disp_v8 && disp_v8->driver) {
        disp_v8->driver->rounder_cb = bsp_lvgl_rounder_cb;
    }
#endif

    result.disp = disp;
    result.indev = indev;

    bsp_display_brightness_init();
    ESP_ERROR_CHECK(esp_lv_adapter_start());
    return result;
}

bsp_lvgl_t bsp_get_broolesia_display(void) {
    if(result.disp == NULL || result.indev == NULL) {
        ESP_LOGE(TAG, "Display not initialized yet");
        return (bsp_lvgl_t){0};
    }
    return result;
}

i2c_master_bus_handle_t bsp_i2c_get_handle(void) {
    if(i2c_bus == nullptr) {
        ESP_LOGE(TAG, "I2C bus not initialized yet");
        return nullptr;
    }
    return i2c_bus->Get_I2cBusHandle();
}

esp_err_t bsp_audio_init(const char *strName) {
    if(i2s_initialized)
    return ESP_OK;
    set_codec_board_type(strName);
  	codec_init_cfg_t codec_cfg = {};
		codec_cfg.in_mode = CODEC_I2S_MODE_TDM;
		codec_cfg.out_mode = CODEC_I2S_MODE_TDM;
		codec_cfg.in_use_tdm = false;
		codec_cfg.reuse_dev = false;
  	ESP_ERROR_CHECK(init_codec(&codec_cfg));
    i2s_initialized = true;
    return ESP_OK;
}

esp_codec_dev_handle_t bsp_audio_codec_speaker_init(void)
{
    ESP_ERROR_CHECK(bsp_audio_init("S3_AMOLED_1_43C"));
  	return get_playback_handle();
}

esp_codec_dev_handle_t bsp_audio_codec_microphone_init(void)
{
    ESP_ERROR_CHECK(bsp_audio_init("S3_AMOLED_1_43C"));
    return get_record_handle();
}

esp_err_t bsp_spiffs_mount(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = "storage",
        .max_files = 2,
        .format_if_mount_failed = true,
    };
    esp_err_t ret_val = esp_vfs_spiffs_register(&conf);
    ESP_ERROR_CHECK(ret_val);
    size_t total = 0, used = 0;
    ret_val = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret_val != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret_val));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
    return ret_val;
}
