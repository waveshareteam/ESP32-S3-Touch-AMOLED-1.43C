#include "round_ui.hpp"
/* SPDX-FileCopyrightText: 2026 Waveshare
 * SPDX-License-Identifier: Apache-2.0 */
#include "XiaozhiUi.hpp"
#include <cstdio>
#include <cstring>

/* Full common Chinese fonts, linked into the app: no SD/SPIFFS dependency. */
LV_FONT_DECLARE(font_puhui_16_4);

namespace esp_brookesia::apps {
namespace {
void plain(lv_obj_t *object)
{
    lv_obj_remove_style_all(object);
    lv_obj_remove_flag(object, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_text_color(object, lv_color_hex(0x1d2b42), 0);
    lv_obj_set_style_text_font(object, &lv_font_montserrat_20, 0);
}
lv_obj_t *box(lv_obj_t *parent, int x, int y, int w, int h, bool black = false)
{
    auto *object = lv_obj_create(parent); plain(object);
    lv_obj_set_pos(object, round_ui::px(x), round_ui::px(y)); lv_obj_set_size(object, round_ui::px(w), round_ui::px(h));
    lv_obj_set_style_bg_color(object, black ? lv_color_black() : lv_color_white(), 0);
    lv_obj_set_style_bg_opa(object, LV_OPA_COVER, 0);
    return object;
}
lv_obj_t *label(lv_obj_t *parent, const char *value, int x, int y, int w, const lv_font_t *font)
{
    auto *object = lv_label_create(parent); plain(object);
    lv_obj_set_pos(object, round_ui::px(x), round_ui::px(y)); lv_obj_set_width(object, round_ui::px(w));
    lv_obj_set_style_text_font(object, font, 0);
    lv_label_set_long_mode(object, LV_LABEL_LONG_MODE_WRAP);
    lv_label_set_text(object, value);
    return object;
}
bool text(lv_obj_t *object, const char *value)
{
    if (!object) return false;
    value = value ? value : "";
    if (!strcmp(lv_label_get_text(object), value)) return false;
    lv_label_set_text(object, value); return true;
}
void hidden(lv_obj_t *object, bool value)
{
    if (value) lv_obj_add_flag(object, LV_OBJ_FLAG_HIDDEN);
    else lv_obj_remove_flag(object, LV_OBJ_FLAG_HIDDEN);
}
}

bool XiaozhiUi::create(lv_obj_t *parent, ActionCallback action, void *context, bool multiline)
{
    (void)multiline;
    if (!parent) return false;
    destroy(); _action_callback = action; _context = context;
    _root = box(parent, 0, 0, 360, 360);
    auto *title = label(_root, "Xiaozhi", 122, 40, 116, &lv_font_montserrat_26);
    lv_obj_set_height(title, round_ui::px(26));
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
    _network = label(_root, "Offline", 132, 66, 96, &lv_font_montserrat_16);
    lv_obj_set_height(_network, round_ui::px(18));
    lv_obj_set_style_text_align(_network, LV_TEXT_ALIGN_CENTER, 0);
    auto *back = lv_button_create(_root); plain(back);
    lv_obj_set_pos(back, round_ui::px(76), round_ui::px(40)); lv_obj_set_size(back, round_ui::px(36), round_ui::px(36));
    auto *arrow = label(back, LV_SYMBOL_LEFT, 0, 0, 28, &lv_font_montserrat_26);
    lv_obj_set_style_text_align(arrow, LV_TEXT_ALIGN_CENTER, 0); lv_obj_center(arrow);
    lv_obj_add_event_cb(back, onBack, LV_EVENT_CLICKED, this);
    lv_obj_set_style_bg_color(_root, lv_color_hex(0xf3f6fb), 0);

    _face = box(_root, 124, 82, 112, 96);
    lv_obj_set_style_border_width(_face, 3, 0);
    lv_obj_set_style_border_color(_face, lv_color_hex(0x376df6), 0);
    lv_obj_set_style_radius(_face, round_ui::px(22), 0);
    _eyes[0] = box(_face, 25, 22, 10, 20, true);
    _eyes[1] = box(_face, 71, 22, 10, 20, true);
    for (auto *eye : _eyes) lv_obj_set_style_radius(eye, LV_RADIUS_CIRCLE, 0);
    _mouth = box(_face, 36, 64, 34, 4, true);
    _smile = lv_arc_create(_face); plain(_smile);
    lv_obj_set_pos(_smile, round_ui::px(32), round_ui::px(37)); lv_obj_set_size(_smile, round_ui::px(42), round_ui::px(38));
    lv_arc_set_rotation(_smile, 0); lv_arc_set_bg_angles(_smile, 0, 180);
    lv_arc_set_angles(_smile, 0, 180);
    lv_obj_set_style_arc_opa(_smile, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_arc_color(_smile, lv_color_black(), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(_smile, round_ui::px(3), LV_PART_INDICATOR);
    lv_obj_set_style_arc_opa(_smile, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_remove_flag(_smile, LV_OBJ_FLAG_CLICKABLE);
    _status = label(_root, "Preparing", 40, 182, 280, &font_puhui_16_4);
    lv_obj_set_height(_status, round_ui::px(24)); lv_obj_set_style_text_align(_status, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_long_mode(_status, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
    _action = lv_button_create(_root); plain(_action);
    lv_obj_set_pos(_action, round_ui::px(115), round_ui::px(302)); lv_obj_set_size(_action, round_ui::px(130), round_ui::px(36));
    lv_obj_set_style_bg_color(_action, lv_color_hex(0x376df6), 0);
    lv_obj_set_style_bg_opa(_action, LV_OPA_COVER, 0); lv_obj_set_style_radius(_action, round_ui::px(14), 0);
    lv_obj_set_style_border_color(_action, lv_color_hex(0x376df6), 0);
    lv_obj_set_style_border_width(_action, 2, 0);
    _action_label = label(_action, "Talk", 0, 0, 122, &lv_font_montserrat_20);
    lv_obj_set_height(_action_label, round_ui::px(24));
    lv_obj_set_style_text_color(_action_label, lv_color_white(), 0);
    lv_obj_set_style_text_align(_action_label, LV_TEXT_ALIGN_CENTER, 0); lv_obj_center(_action_label);
    lv_obj_add_event_cb(_action, onAction, LV_EVENT_CLICKED, this);

    _chat_panel = box(_root, 56, 210, 248, 80);
    _role = label(_chat_panel, "CONVERSATION", 10, 6, 228, &lv_font_montserrat_16);
    lv_obj_set_height(_role, round_ui::px(18));
    lv_obj_set_style_radius(_chat_panel, round_ui::px(14), 0);
    auto *scroll = _chat_scroll = box(_chat_panel, 10, 26, 228, 48);
    lv_obj_add_flag(scroll, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scroll_dir(scroll, LV_DIR_VER); lv_obj_set_scrollbar_mode(scroll, LV_SCROLLBAR_MODE_ACTIVE);
    lv_obj_set_style_bg_color(scroll, lv_color_black(), LV_PART_SCROLLBAR);
    lv_obj_set_style_bg_opa(scroll, LV_OPA_COVER, LV_PART_SCROLLBAR);
    lv_obj_set_style_width(scroll, 3, LV_PART_SCROLLBAR);
    _chat = label(scroll, "Say \"你好小智\"\nor tap Talk.", 0, 0, 218, &font_puhui_16_4);
    lv_obj_set_style_text_line_space(_chat, round_ui::px(4), 0);

    _activation = box(_root, 56, 88, 248, 202);
    auto *activation_title = label(_activation, "DEVICE ACTIVATION", 10, 12, 228, &lv_font_montserrat_18);
    lv_obj_set_height(activation_title, round_ui::px(22));
    lv_obj_set_style_text_align(activation_title, LV_TEXT_ALIGN_CENTER, 0);
    _code = label(_activation, "", 10, 42, 228, &lv_font_montserrat_42);
    lv_obj_set_height(_code, round_ui::px(42));
    lv_label_set_long_mode(_code, LV_LABEL_LONG_MODE_DOTS);
    lv_obj_set_style_text_align(_code, LV_TEXT_ALIGN_CENTER, 0);
    auto *instructions = box(_activation, 12, 92, 224, 98);
    lv_obj_add_flag(instructions, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scroll_dir(instructions, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(instructions, LV_SCROLLBAR_MODE_AUTO);
    _instruction = label(instructions, "", 0, 0, 214, &font_puhui_16_4);
    lv_obj_set_style_text_align(_instruction, LV_TEXT_ALIGN_CENTER, 0);
    hidden(_activation, true); setEmotion("neutral");
    return true;
}
void XiaozhiUi::destroy()
{
    if (_root) lv_obj_delete(_root);
    _root = _network = _status = _face = _mouth = _smile = nullptr;
    _eyes[0] = _eyes[1] = nullptr;
    _action = _action_label = _chat_panel = _chat_scroll = _role = _chat = nullptr;
    _activation = _code = _instruction = nullptr;
    _emotion[0] = '\0';
}
void XiaozhiUi::setNetworkReady(bool ready) { text(_network, ready ? "Wi-Fi " LV_SYMBOL_OK : "Offline"); }
void XiaozhiUi::setStatus(const char *value) { text(_status, value); }
void XiaozhiUi::setAction(const char *value, bool enabled)
{
    text(_action_label, value);
    if (!_action) return;
    lv_obj_set_style_border_width(_action, enabled ? 2 : 1, 0);
    if (enabled) lv_obj_remove_state(_action, LV_STATE_DISABLED);
    else lv_obj_add_state(_action, LV_STATE_DISABLED);
}
void XiaozhiUi::setChatMessage(const char *role, const char *value)
{
    if (!_chat) return;
    const bool empty = !value || !value[0];
    text(_role, empty ? "CONVERSATION" : (role && !strcmp(role, "user") ? "YOU" : "XIAOZHI"));
    if (text(_chat, empty ? "Say \"你好小智\"\nor tap Talk." : value))
        lv_obj_scroll_to_y(lv_obj_get_parent(_chat), 0, LV_ANIM_OFF);
}
void XiaozhiUi::clearChatMessages() { setChatMessage(nullptr, ""); }
void XiaozhiUi::setEmotion(const char *value)
{
    if (!_face) return;
    value = value && value[0] ? value : "neutral";
    if (!strcmp(_emotion, value)) return;
    snprintf(_emotion, sizeof(_emotion), "%s", value);
    const bool happy = !strcmp(value,"happy") || !strcmp(value,"laughing") || !strcmp(value,"loving") || !strcmp(value,"funny");
    const bool sad = !strcmp(value,"sad") || !strcmp(value,"crying") || !strcmp(value,"angry");
    const bool surprised = !strcmp(value,"surprised") || !strcmp(value,"shocked");
    hidden(_smile, !(happy || sad)); hidden(_mouth, happy || sad);
    lv_arc_set_rotation(_smile, sad ? 180 : 0);
    lv_obj_set_pos(_smile, round_ui::px(32), round_ui::px(sad ? 52 : 37));
    lv_obj_set_size(_mouth, round_ui::px(surprised ? 14 : 34), round_ui::px(surprised ? 18 : 4));
    lv_obj_set_pos(_mouth, round_ui::px(surprised ? 46 : 36), round_ui::px(surprised ? 57 : 64));
    lv_obj_set_style_radius(_mouth, surprised ? LV_RADIUS_CIRCLE : 0, 0);
    for (int i=0;i<2;i++) {
        const bool closed = !strcmp(value,"sleepy") || !strcmp(value,"relaxed") || (!strcmp(value,"winking") && i==1);
        lv_obj_set_size(_eyes[i], round_ui::px(closed ? 17 : 10), round_ui::px(closed ? 4 : 20));
        lv_obj_set_y(_eyes[i], round_ui::px(closed ? 31 : 22));
    }
}
void XiaozhiUi::setActivation(const char *code, const char *message, bool visible)
{
    if (!_activation) return;
    const bool code_changed = text(_code, code);
    const bool message_changed = text(_instruction, message);
    if (code_changed || message_changed)
        lv_obj_scroll_to_y(lv_obj_get_parent(_instruction), 0, LV_ANIM_OFF);
    hidden(_activation, !visible);
}
void XiaozhiUi::onAction(lv_event_t *event)
{
    auto *self = static_cast<XiaozhiUi *>(lv_event_get_user_data(event));
    if (self->_action_callback) self->_action_callback(self->_context);
}
void XiaozhiUi::onBack(lv_event_t *event)
{
    auto *self = static_cast<XiaozhiUi *>(lv_event_get_user_data(event));
    if (self->_back_callback) self->_back_callback(self->_context);
}
}
