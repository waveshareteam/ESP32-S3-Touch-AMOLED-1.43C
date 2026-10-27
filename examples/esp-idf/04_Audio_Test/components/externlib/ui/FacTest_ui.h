#ifndef FACTEST_UI
#define FACTEST_UI
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

typedef struct
{
	lv_obj_t *screen;
	bool screen_del;
	lv_obj_t *screen_cont_1;
	lv_obj_t *screen_label_1;
	lv_obj_t *screen_label_2;
}lv_factest_ui;

void setup_factest_ui(lv_factest_ui *ui);

LV_FONT_DECLARE(lv_font_montserratMedium_30)
LV_FONT_DECLARE(lv_font_montserratMedium_16)


#ifdef __cplusplus
}
#endif

#endif