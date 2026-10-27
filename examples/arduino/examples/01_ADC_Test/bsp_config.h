#ifndef USER_CONFIG_H
#define USER_CONFIG_H

#define BSP_SPI_HOST        SPI2_HOST
#define BSP_I2C_HOST        I2C_NUM_0

#define ESP32_I2C_SDA       GPIO_NUM_47
#define ESP32_I2C_SCL       GPIO_NUM_48

#define LCD_WIDTH           466    
#define LCD_HEIGHT          466    

#define LCD_D0_PIN          GPIO_NUM_9
#define LCD_D1_PIN          GPIO_NUM_10
#define LCD_D2_PIN          GPIO_NUM_11
#define LCD_D3_PIN          GPIO_NUM_12
#define LCD_CS_PIN          GPIO_NUM_15
#define LCD_SCK_PIN         GPIO_NUM_14
#define LCD_RST_PIN         GPIO_NUM_13
#define LCD_TE_PIN          GPIO_NUM_8

#define TP_RST_PIN          GPIO_NUM_16
#define TP_INT_PIN          GPIO_NUM_17
#define ESP32_I2C_SDA_PIN   ESP32_I2C_SDA
#define ESP32_I2C_SCL_PIN   ESP32_I2C_SCL

/*
电路Codec引脚说明
*/
#define CODEC_I2S_NUM               I2S_NUM_0
#define CODEC_I2S_MCLK_PIN          GPIO_NUM_38
#define CODEC_I2S_SCLK_PIN          GPIO_NUM_39
#define CODEC_I2S_LRCK_PIN          GPIO_NUM_40
#define CODEC_I2S_DSDIN_PIN         GPIO_NUM_41
#define CODEC_I2S_DSOUT_PIN         GPIO_NUM_42
#define CODEC_NS415_CTRL_PIN        GPIO_NUM_46
#define CODEC_LDO_EN_PIN            GPIO_NUM_18

/*
BAT ADC
*/
#define BAT_ADC_GPIO_PIN            GPIO_NUM_4
#define BAT_LED_GPIO_PIN            GPIO_NUM_5
#define BAT_ETA6098_STAT_PIN        GPIO_NUM_7


/*
BUTTON 
*/
#define BUTTON_0_GPIO_PIN           GPIO_NUM_0

/*lvgl*/
#define LVGL_TICK_PERIOD_MS    2
#define LVGL_TASK_MAX_DELAY_MS 500
#define LVGL_TASK_MIN_DELAY_MS 5
#define LVGL_TASK_STACK_SIZE   (8 * 1024)
#define LVGL_TASK_PRIORITY     2

#endif 