/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "systems/phone/widgets/status_bar/esp_brookesia_status_bar.hpp"
#include "systems/phone/assets/esp_brookesia_phone_assets.h"

namespace esp_brookesia::systems::phone {

constexpr StatusBar::AreaData STYLESHEET_466_466_DARK_STATUS_BAR_AREA_DATA(int w_percent, StatusBar::AreaAlign align)
{
    return {
        .size = gui::StyleSize::RECT_PERCENT(w_percent, 100),
        .layout_column_align = align,
        .layout_column_start_offset = 0,
        .layout_column_pad = 13,
    };
}

constexpr StatusBar::Data STYLESHEET_466_466_DARK_STATUS_BAR_DATA = {
    .main = {
        .size = gui::StyleSize::RECT(466, 104),
        .size_min = gui::StyleSize::RECT_W_PERCENT(100, 31),
        .size_max = gui::StyleSize::RECT_W_PERCENT(100, 78),
        .background_color = gui::StyleColor::COLOR_WITH_OPACITY(0, 0),
        .text_font = gui::StyleFont::SIZE(28),
        .text_color = gui::StyleColor::COLOR(0xFFFFFF),
    },
    .area = {

        .num = 1,
        .data = {
            STYLESHEET_466_466_DARK_STATUS_BAR_AREA_DATA(100, StatusBar::AreaAlign::CENTER),
        },
    },
    .icon_common_size = gui::StyleSize::SQUARE(34),
    .battery = {
        .area_index = 0,
        .icon_data = {
            .icon = {
                .image_num = 5,
                .images = {
                    gui::StyleImage::IMAGE_RECOLOR_WHITE(&esp_brookesia_image_large_status_bar_battery_level1_36_36),
                    gui::StyleImage::IMAGE_RECOLOR_WHITE(&esp_brookesia_image_large_status_bar_battery_level2_36_36),
                    gui::StyleImage::IMAGE_RECOLOR_WHITE(&esp_brookesia_image_large_status_bar_battery_level3_36_36),
                    gui::StyleImage::IMAGE_RECOLOR_WHITE(&esp_brookesia_image_large_status_bar_battery_level4_36_36),
                    gui::StyleImage::IMAGE_RECOLOR_WHITE(&esp_brookesia_image_large_status_bar_battery_charge_36_36),
                },
            },
        },
    },
    .wifi = {
        .area_index = 0,
        .icon_data = {
            .icon = {
                .image_num = 4,
                .images = {
                    gui::StyleImage::IMAGE_RECOLOR_WHITE(&esp_brookesia_image_large_status_bar_wifi_close_36_36),
                    gui::StyleImage::IMAGE_RECOLOR_WHITE(&esp_brookesia_image_large_status_bar_wifi_level1_36_36),
                    gui::StyleImage::IMAGE_RECOLOR_WHITE(&esp_brookesia_image_large_status_bar_wifi_level2_36_36),
                    gui::StyleImage::IMAGE_RECOLOR_WHITE(&esp_brookesia_image_large_status_bar_wifi_level3_36_36),
                },
            },
        },
    },
    .clock = {
        .area_index = 0,
    },
    .flags = {
        .enable_main_size_min = 0,
        .enable_main_size_max = 0,
        .enable_battery_icon = 0,
        .enable_battery_icon_common_size = 1,
        .enable_battery_label = 0,
        .enable_wifi_icon = 1,
        .enable_wifi_icon_common_size = 1,
        .enable_clock = 1,
    },
};

} // namespace esp_brookesia::systems::phone
