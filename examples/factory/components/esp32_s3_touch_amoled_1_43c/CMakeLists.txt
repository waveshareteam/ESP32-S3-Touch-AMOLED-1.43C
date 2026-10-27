idf_component_register(
    SRCS "esp32_s3_touch_amoled_1_43c.cpp" "bsp_board.cpp" "esp_lcd_touch_cst820.cpp"
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "priv_include"
    REQUIRES driver esp_lcd esp_lcd_co5300 esp_lcd_touch
             esp_lvgl_adapter lvgl esp_codec_dev
             esp_wifi esp_netif
    PRIV_REQUIRES bsp_extra button nvs_flash esp_timer esp_event)

# Keep hardware initialization separate from the APP audio-session layer.
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-missing-field-initializers)
