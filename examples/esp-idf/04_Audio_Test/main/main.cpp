#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <esp_log.h>
#include "esp32_s3_touch_amoled_1.43c.h"

#include "lvgl.h"
#include "lv_demos.h"
#include "FacTest_ui.h"
#include "button.h"
#include "bsp_config.h"


#define TAG   "main"

extern const uint8_t music_pcm_start[] asm("_binary_mood_30_pcm_start");
extern const uint8_t music_pcm_end[]   asm("_binary_mood_30_pcm_end");


lv_factest_ui src_ui;
static esp_codec_dev_handle_t speaker;
static esp_codec_dev_handle_t microphone;
static Button *boot_button_ = NULL;
static EventGroupHandle_t FacTestEventGroup = NULL;     // 事件组句柄
static uint8_t *data_ptr = NULL;
#define DATA_PTR_MAX 192000
static bool is_status = 1;
static bool is_musicflag = 0;
/*
单击:录音3s+播放
长按:播放音乐
*/
void InitializeButtons(void) {
    boot_button_->OnClick([]() {
        if(1 == is_status) {
            xEventGroupSetBits(FacTestEventGroup, 0x01);
        }
    });

    boot_button_->OnDoubleClick([]() {
        if(1 == is_status) {
            is_musicflag = 1;
            xEventGroupSetBits(FacTestEventGroup, 0x02);
        }
    });

    boot_button_->OnLongPress([]() {
        if(1 == is_musicflag) {
            is_musicflag = 0;
        }
    });
}

void Audio_LoopTask(void *arg) {
    data_ptr = (uint8_t *)heap_caps_malloc(DATA_PTR_MAX, MALLOC_CAP_SPIRAM);
    assert(data_ptr);
    esp_codec_dev_sample_info_t fs = {};
    fs.sample_rate = 16000;
    fs.channel = 2;
    fs.bits_per_sample = 16;
    ESP_ERROR_CHECK(esp_codec_dev_open(speaker, &fs));
  	ESP_ERROR_CHECK(esp_codec_dev_open(microphone, &fs));  
    ESP_ERROR_CHECK(esp_codec_dev_set_out_vol(speaker, 100));
    ESP_ERROR_CHECK(esp_codec_dev_set_in_gain(microphone,35));
    while(1) {
        EventBits_t even = xEventGroupWaitBits(FacTestEventGroup,(0x01) | (0x02),pdTRUE,pdFALSE,portMAX_DELAY);
        is_status = 0;
        if(even & 0x01) {
            if(bsp_display_lock(-1) == ESP_OK) {
                lv_label_set_text(src_ui.screen_label_1, "Recording in progress");
                bsp_display_unlock();
            }
            ESP_ERROR_CHECK(esp_codec_dev_read(microphone, data_ptr, DATA_PTR_MAX));
            if(bsp_display_lock(-1) == ESP_OK) {
                lv_label_set_text(src_ui.screen_label_1, "Playing the recording");
                bsp_display_unlock();
            }
            ESP_ERROR_CHECK(esp_codec_dev_write(speaker, data_ptr, DATA_PTR_MAX));
            if(bsp_display_lock(-1) == ESP_OK) {
                lv_label_set_text(src_ui.screen_label_1, "Playback completed");
                bsp_display_unlock();
            }
        }
        else if(even & 0x02) {
            if(bsp_display_lock(-1) == ESP_OK) {
                lv_label_set_text(src_ui.screen_label_1, "Playing music");
                bsp_display_unlock();
            }
            size_t bytes_write = 0;
            uint8_t *music_buffer = (uint8_t *)music_pcm_start;
            size_t bytes_max = music_pcm_end - music_pcm_start;
            while((bytes_write < bytes_max) && (is_musicflag)) {
                ESP_ERROR_CHECK(esp_codec_dev_write(speaker, music_buffer, 256));
      		    music_buffer += 256;
      		    bytes_write += 256;
            }
            if(bsp_display_lock(-1) == ESP_OK) {
                lv_label_set_text(src_ui.screen_label_1, "Playback completed");
                bsp_display_unlock();
            }
        }
        is_status = 1;
    }
}

extern "C" void app_main(void) {
    FacTestEventGroup = xEventGroupCreate(); /* 创建事件组 */
    bsp_broolesia_display_init();
    speaker = bsp_audio_codec_speaker_init();
    microphone = bsp_audio_codec_microphone_init();
    boot_button_ = new Button(BUTTON_0_GPIO_PIN);
    InitializeButtons();
    if(bsp_display_lock(-1) == ESP_OK) {
        setup_factest_ui(&src_ui);
        bsp_display_unlock();
    }
    xTaskCreatePinnedToCore(Audio_LoopTask, "Audio_LoopTask", 5 * 1024, NULL, 5, NULL,1);
}


