#pragma once
#include "bsp/esp-bsp.h"
namespace round_ui {
constexpr int BASE_SIZE = 360;
constexpr int px(int value) {
    return value < 0 ? -((-value * BSP_LCD_H_RES + BASE_SIZE / 2) / BASE_SIZE)
                     : (value * BSP_LCD_H_RES + BASE_SIZE / 2) / BASE_SIZE;
}
}
