idf_component_register(SRCS "main.cpp" "dark/scr_bg.c" INCLUDE_DIRS "."
    REQUIRES esp32_s3_touch_amoled_1_43c brookesia_core Settings AIChats DrawingBoard
             Weather Music Games MicTest ButtonTest DisplayTest
             network_service esp_timer)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-missing-field-initializers)
