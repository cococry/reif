#pragma once

#include "layout.h"
#include "ui_core.h"
#include "util.h"
#include "widgets/div.h"
#include "widgets/button.h"
#include "widgets/image.h"
#include "widgets/input.h"
#include "widgets/text.h"
#include "widgets/slider.h"

typedef enum {
  LF_TEXT_LEVEL_H1 = 0,
  LF_TEXT_LEVEL_H2 = 1,
  LF_TEXT_LEVEL_H3 = 2,
  LF_TEXT_LEVEL_H4 = 3,
  LF_TEXT_LEVEL_H5 = 4,
  LF_TEXT_LEVEL_H6 = 5,
  LF_TEXT_LEVEL_PARGRAPH = 6
} lf_text_level;

#define lf_style_widget_prop(ui, widget, prop, val) \
  do { \
    lf_widget_set_prop((ui), (widget), &(widget)->props.prop, (val)); \
    (widget)->_initial_props.prop = (val); \
    (widget)->_rendered_props.prop = (val); \
    if(!ui->_ez._assignment_only) {         \
      (widget)->_component_props = (widget)->props; \
    } \
    if((widget)->_component_props.prop != (widget)->_rendered_props.prop) { \
      (widget)->_component_props.prop = (val); \
    } \
  } while(0)

#define lf_style_widget_prop_color(ui, widget, prop, val) \
  do { \
    lf_widget_set_prop_color((ui), (widget), &(widget)->props.prop, (val)); \
    (widget)->_initial_props.prop = (val); \
    (widget)->_rendered_props.prop = (val); \
    if(!ui->_ez._assignment_only) { \
      (widget)->_component_props = (widget)->props; \
    } \
    if( \
        (widget)->_component_props.prop.r != (widget)->_rendered_props.prop.r || \
        (widget)->_component_props.prop.g != (widget)->_rendered_props.prop.g || \
        (widget)->_component_props.prop.b != (widget)->_rendered_props.prop.b || \
        (widget)->_component_props.prop.a != (widget)->_rendered_props.prop.a ) { \
      (widget)->_component_props.prop = (val); \
    } \
  } while(0)


lf_ez_state_t lf_ez_api_init(lf_ui_state_t* ui);

void lf_ez_api_terminate(lf_ui_state_t* ui);

void lf_ez_api_set_assignment_only_mode(lf_ui_state_t* ui, bool assignment_only);

lf_div_t* lf_div(lf_ui_state_t* ui);

void lf_div_end(lf_ui_state_t* ui);

void lf_slider_end(lf_ui_state_t* ui);

lf_button_t* lf_button(lf_ui_state_t* ui);

void lf_button_end(lf_ui_state_t* ui);

lf_slider_t* lf_slider(lf_ui_state_t* ui, float* val, float min, float max);

lf_text_t* lf_text_p(lf_ui_state_t* ui, const char* label);

lf_text_t* lf_text_h1(lf_ui_state_t* ui, const char* label);

lf_text_t* lf_text_h2(lf_ui_state_t* ui, const char* label);

lf_text_t* lf_text_h3(lf_ui_state_t* ui, const char* label);

lf_text_t* lf_text_h4(lf_ui_state_t* ui, const char* label);

lf_text_t* lf_text_h5(lf_ui_state_t* ui, const char* label);

lf_text_t* lf_text_h6(lf_ui_state_t* ui, const char* label);

lf_text_t*  lf_text_sized(lf_ui_state_t* ui, const char* label, uint32_t pixel_size);

lf_text_dimension_t lf_text_measure(lf_ui_state_t* ui, const char* text, lf_font_t font);

lf_widget_t* lf_crnt(lf_ui_state_t* ui);

lf_image_t* lf_image(lf_ui_state_t* ui, const char* filepath);

lf_image_t* lf_image_sized(lf_ui_state_t* ui, const char* filepath, uint32_t w, uint32_t h);

lf_image_t* lf_image_sized_w(lf_ui_state_t* ui, const char* filepath, uint32_t w);

lf_image_t* lf_image_sized_h(lf_ui_state_t* ui, const char* filepath, uint32_t w);

void lf_component(lf_ui_state_t* ui, lf_component_func_t comp_func);

void lf_component_rerender(lf_ui_state_t* ui, lf_component_func_t comp_func);

void lf_skip_widget(lf_ui_state_t* ui);

void lf_content_box(lf_ui_state_t* ui);

void lf_content_box_end(lf_ui_state_t* ui);

void lf_grower(lf_ui_state_t* ui);

lf_input_t* lf_input(lf_ui_state_t* ui, char* buf);
