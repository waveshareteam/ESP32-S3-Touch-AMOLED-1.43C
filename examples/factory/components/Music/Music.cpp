#include "round_ui.hpp"
#include "bsp_board_extra.h"
#include "Music.hpp"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <new>
LV_IMAGE_DECLARE(img_app_musicplayer);
LV_IMAGE_DECLARE(img_app_music_cover_1);
using namespace app_ui;
namespace esp_brookesia::apps {
Music *Music::instance_;
Music *Music::requestInstance() { if(!instance_) instance_=new(std::nothrow) Music; return instance_; }
Music::Music():App("Music",&img_app_musicplayer,true,false,false) {}
Music::~Music() { close(); instance_=nullptr; }
bool Music::run() {
    root_=screen("Now Playing",event,this);
    count_=label(root_,"0 / 0",132,76,96,&lv_font_montserrat_16,MUTED);
    lv_obj_set_style_text_align(count_,LV_TEXT_ALIGN_CENTER,0);
    auto *cover=lv_image_create(root_); lv_image_set_src(cover,&img_app_music_cover_1);
    lv_image_set_pivot(cover,0,0); lv_image_set_scale(cover,round_ui::px(128));
    lv_obj_set_pos(cover, (BSP_LCD_H_RES-img_app_music_cover_1.header.w*round_ui::px(128)/256)/2, round_ui::px(98));
    title_=label(root_,"Local music",56,208,248,&lv_font_source_han_sans_sc_16_cjk);
    lv_obj_set_style_text_align(title_,LV_TEXT_ALIGN_CENTER,0); lv_label_set_long_mode(title_,LV_LABEL_LONG_MODE_DOTS);
    state_=label(root_,"Built-in music",56,234,248,&lv_font_montserrat_16,MUTED);
    lv_obj_set_style_text_align(state_,LV_TEXT_ALIGN_CENTER,0);
    progress_=lv_bar_create(root_); lv_obj_set_pos(progress_, round_ui::px(70), round_ui::px(258)); lv_obj_set_size(progress_, round_ui::px(220), round_ui::px(5)); lv_bar_set_range(progress_,0,1000);
    lv_obj_set_style_bg_color(progress_,lv_color_hex(0xdce4f1),0); lv_obj_set_style_bg_color(progress_,lv_color_hex(BLUE),LV_PART_INDICATOR);
    elapsed_=label(root_,"00:00",70,270,70,&lv_font_montserrat_16,MUTED);
    duration_=label(root_,"00:00",220,270,70,&lv_font_montserrat_16,MUTED); lv_obj_set_style_text_align(duration_,LV_TEXT_ALIGN_CENTER,0);
    button(root_,LV_SYMBOL_PREV,82,296,48,38,1,event,this);
    auto *play=button(root_,LV_SYMBOL_PLAY,152,290,56,48,2,event,this,true); play_label_=lv_obj_get_child(play,0);
    button(root_,LV_SYMBOL_NEXT,230,296,48,38,3,event,this);
    play_=false; selected_=0; view_=View{};
    timer_=lv_timer_create(poll,200,this);
    if(!timer_ || !start("music",8192,4)) { close(); return false; }
    return true;
}
extern const uint8_t demo_start[] asm("_binary_demo_pcm_start");
extern const uint8_t demo_end[] asm("_binary_demo_pcm_end");
static void music_format_time(uint32_t ms, char *out, size_t size) {
    snprintf(out, size, "%02lu:%02lu", (unsigned long)(ms/60000), (unsigned long)(ms/1000%60));
}
void Music::work() {
    constexpr uint32_t RATE=16000, FRAME_BYTES=4;
    const size_t frames=(demo_end-demo_start)/FRAME_BYTES;
    View v; v.count=1; v.duration=frames*1000/RATE; v.ready=true;
    snprintf(v.title,sizeof(v.title),"Built-in demo");
    snprintf(v.status,sizeof(v.status),"Tap Play to listen"); publish(v);
    size_t cursor=0; unsigned selection=selected_.load(); bool audio=false;
    int16_t pcm[512];
    auto close_audio=[&]() {
        if(audio) { bsp_extra_audio_drain(); bsp_extra_codec_dev_stop(); bsp_extra_audio_release(); audio=false; }
    };
    while(!stopping()) {
        if(selected_.load()!=selection) { selection=selected_.load(); cursor=0; v.position=0; }
        if(!play_.load()) {
            close_audio(); v.playing=false; publish(v); vTaskDelay(pdMS_TO_TICKS(20)); continue;
        }
        if(cursor==frames) cursor=0;
        if(!audio) {
            audio=bsp_extra_audio_acquire(BSP_EXTRA_AUDIO_OWNER_MUSIC,100);
            esp_err_t ret=audio?bsp_extra_codec_set_fs(RATE,16,I2S_SLOT_MODE_MONO):ESP_ERR_TIMEOUT;
            if(ret!=ESP_OK) {
                close_audio(); play_=false; v.playing=false;
                snprintf(v.status,sizeof(v.status),"Audio busy; close other audio APPs"); publish(v); continue;
            }
        }
        const size_t count=std::min<size_t>(512,frames-cursor);
        const auto *samples=reinterpret_cast<const int16_t *>(demo_start)+cursor*2;
        for(size_t i=0;i<count;++i) pcm[i]=(int32_t(samples[i*2])+samples[i*2+1])/2;
        size_t written=0;
        esp_err_t ret=bsp_extra_i2s_write(pcm,count*sizeof(int16_t),&written,1000);
        if(ret!=ESP_OK || written!=count*sizeof(int16_t)) {
            close_audio(); play_=false; v.playing=false;
            snprintf(v.status,sizeof(v.status),"Audio write failed; tap Play to retry"); publish(v); continue;
        }
        cursor+=count; v.position=cursor*1000/RATE; v.playing=true;
        snprintf(v.status,sizeof(v.status),"Playing built-in music");
        if(cursor==frames) { close_audio(); play_=false; v.playing=false; snprintf(v.status,sizeof(v.status),"Finished / Tap Play to replay"); }
        publish(v);
    }
    close_audio();
}
void Music::refresh() {
    View v; portENTER_CRITICAL(&lock_); v=view_; portEXIT_CRITICAL(&lock_);
    text(title_,v.title); text(state_,v.status); text(play_label_,v.playing?LV_SYMBOL_PAUSE:LV_SYMBOL_PLAY);
    char s[32]; snprintf(s,sizeof(s),"%02u / %02u",v.count?v.index+1:0,v.count); text(count_,s);
    music_format_time(v.position,s,sizeof(s)); text(elapsed_,s);
    music_format_time(v.duration,s,sizeof(s)); text(duration_,s);
    lv_bar_set_value(progress_,v.duration?(uint64_t)v.position*1000/v.duration:0,LV_ANIM_OFF);
}
void Music::event(lv_event_t *e) {
    auto *a=static_cast<Music *>(lv_event_get_user_data(e)); int id=action(e);
    if(id==0) { a->back(); return; }
    View v; portENTER_CRITICAL(&a->lock_); v=a->view_; portEXIT_CRITICAL(&a->lock_);
    if(id==2) {
        if(v.ready) a->play_=!a->play_.load();
        else {
            a->stop(); a->play_=false;
            v.playing=false; snprintf(v.status,sizeof(v.status),"Built-in music"); a->publish(v);
            if(!a->start("music",8192,4)) { snprintf(v.status,sizeof(v.status),"Not enough memory. Reopen to retry."); a->publish(v); }
        }
    }
    else if(v.count) ++a->selected_;
}
void Music::poll(lv_timer_t *t) { static_cast<Music *>(lv_timer_get_user_data(t))->refresh(); }
bool Music::pause() { play_=false; if(timer_) lv_timer_pause(timer_); stop(); return true; }
bool Music::resume() { play_=false; if(timer_) lv_timer_resume(timer_); return start("music",8192,4); }
bool Music::close() { play_=false; stop(); if(timer_) { lv_timer_delete(timer_); timer_=nullptr; } if(root_) lv_obj_delete(root_); root_=nullptr; return true; }
}
