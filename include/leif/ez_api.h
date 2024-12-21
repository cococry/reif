#pragma once

#include "layout.h"
#include "ui_core.h"
#include "util.h"
#include "widgets/div.h"
#include "widgets/button.h"
#include "widgets/image.h"
#include "widgets/text.h"


#define lf_style_crnt_widget_prop(ui, prop, val) \
  (ui->_current_widget->props.prop = (val), lf_widget_submit_props((ui->_current_widget)))

#define lf_style_widget_prop(ui, widget, prop, val) \
  ((widget)->props.prop = (val), lf_widget_submit_props(widget))

lf_div_t* lf_div(lf_ui_state_t* ui);

void lf_div_end(lf_ui_state_t* ui);

lf_button_t* lf_button(lf_ui_state_t* ui);

void lf_button_end(lf_ui_state_t* ui);

lf_text_t* lf_text_p(lf_ui_state_t* ui, const char* label);

lf_text_t* lf_text_h1(lf_ui_state_t* ui, const char* label);

lf_text_t* lf_text_h2(lf_ui_state_t* ui, const char* label);

lf_text_t* lf_text_h3(lf_ui_state_t* ui, const char* label);

lf_text_t* lf_text_h4(lf_ui_state_t* ui, const char* label);

lf_text_t* lf_text_h5(lf_ui_state_t* ui, const char* label);

lf_text_t* lf_text_h6(lf_ui_state_t* ui, const char* label);

lf_text_t* lf_text_custom_font(lf_ui_state_t* ui, const char* label, lf_font_t font);

lf_text_dimension_t lf_text_measure(lf_ui_state_t* ui, const char* text, lf_font_t font);

lf_widget_t* lf_crnt(lf_ui_state_t* ui);

lf_image_t* lf_image(lf_ui_state_t* ui, const char* filepath);
