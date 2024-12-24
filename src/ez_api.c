#include "../include/leif/ez_api.h"

static void _end_widget(lf_ui_state_t* ui);

void 
_end_widget(lf_ui_state_t* ui) {
  if(!ui->_last_parent || !ui->_last_parent->parent) return;
  ui->_last_parent = ui->_last_parent->parent;
}

lf_div_t*
lf_div(lf_ui_state_t* ui) {
  if(!ui->_last_parent) return NULL;
  lf_div_t* div = lf_div_create(ui, ui->_last_parent);
  ui->_last_parent = &div->base;
  ui->_current_widget = &div->base;
  return div;
}

void 
lf_div_end(lf_ui_state_t* ui) {
  _end_widget(ui);
}

lf_button_t*
lf_button(lf_ui_state_t* ui) {
  lf_button_t* btn = lf_button_create(ui, ui->_last_parent);
  ui->_last_parent = &btn->base;
  ui->_current_widget = &btn->base;
  return btn;
}

void
lf_button_end(lf_ui_state_t* ui) {
  _end_widget(ui);
}

lf_text_t* 
lf_text_p(lf_ui_state_t* ui, const char* label) {
  lf_text_t* txt = lf_text_create(ui, ui->_last_parent, label);
  ui->_current_widget = &txt->base;
  return txt;
}


lf_text_t* 
lf_text_h1(lf_ui_state_t* ui, const char* label) {
  lf_text_t* txt = lf_text_create_ex(ui, ui->_last_parent, label, ui->fonts[TextLevelH1]);
  ui->_current_widget = &txt->base;
  return txt;
}

lf_text_t* 
lf_text_h2(lf_ui_state_t* ui, const char* label) {
  lf_text_t* txt = lf_text_create_ex(ui, ui->_last_parent, label, ui->fonts[TextLevelH2]);
  ui->_current_widget = &txt->base;
  return txt;
}

lf_text_t*
lf_text_h3(lf_ui_state_t* ui, const char* label) {
  lf_text_t* txt = lf_text_create_ex(ui, ui->_last_parent, label, ui->fonts[TextLevelH3]);
  ui->_current_widget = &txt->base;
  return txt;
}

lf_text_t*
lf_text_h4(lf_ui_state_t* ui, const char* label) {
  lf_text_t* txt = lf_text_create_ex(ui, ui->_last_parent, label, ui->fonts[TextLevelH4]);
  ui->_current_widget = &txt->base;
  return txt;
}

lf_text_t*
lf_text_h5(lf_ui_state_t* ui, const char* label) {
  lf_text_t* txt = lf_text_create_ex(ui, ui->_last_parent, label, ui->fonts[TextLevelH5]);
  ui->_current_widget = &txt->base;
  return txt;
}

lf_text_t* 
lf_text_h6(lf_ui_state_t* ui, const char* label) {
  lf_text_t* txt = lf_text_create_ex(ui, ui->_last_parent, label, ui->fonts[TextLevelH6]);
  ui->_current_widget = &txt->base;
  return txt;
}

lf_text_t* 
lf_text_custom_font(lf_ui_state_t* ui, const char* label, lf_font_t font) {
  lf_text_t* txt = lf_text_create_ex(ui, ui->_last_parent, label, font); 
  ui->_current_widget = &txt->base;
  return txt;
}

lf_text_dimension_t 
lf_text_measure(lf_ui_state_t* ui, const char* text, lf_font_t font) {
  return ui->render_get_text_dimension(ui->render_state, text, font);
}

lf_widget_t*
lf_crnt(lf_ui_state_t* ui) {
  return ui->_current_widget;
}

lf_image_t* 
lf_image(lf_ui_state_t* ui, const char* filepath) {
  return lf_image_create(ui, ui->_last_parent, filepath);
}

lf_image_t* 
lf_image_ex(lf_ui_state_t* ui, const char* filepath, uint32_t w, uint32_t h) {
  return lf_image_create_ex(ui, ui->_last_parent, filepath, w, h);
}
