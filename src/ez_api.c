#include "../include/leif/ez_api.h"

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
  if(!ui->_last_parent || !ui->_last_parent->parent)  return;
  ui->_last_parent = ui->_last_parent->parent;
}

lf_button_t*
lf_button(lf_ui_state_t* ui, const char* label) {
  lf_button_t* btn = lf_button_create_with_label(ui, ui->_last_parent, label);
  ui->_current_widget = &btn->base;
  return btn;
}

lf_button_t* 
lf_button_empty(lf_ui_state_t* ui) {
  lf_button_t* btn = lf_button_create(ui, ui->_last_parent);
  ui->_current_widget = &btn->base;
  return btn;
}

lf_text_t* 
lf_text_p(lf_ui_state_t* ui, const char* label) {
  lf_text_t* txt = lf_text_create(ui, ui->_last_parent, label);
  ui->_current_widget = &txt->base;
  return txt;
}


lf_text_t* 
lf_text_h1(lf_ui_state_t* ui, const char* label) {
  lf_text_t* txt = lf_text_create_ex(ui, ui->_last_parent, label, ui->font_h1);
  ui->_current_widget = &txt->base;
  return txt;
}

lf_text_t* 
lf_text_h2(lf_ui_state_t* ui, const char* label) {
  lf_text_t* txt = lf_text_create_ex(ui, ui->_last_parent, label, ui->font_h2);
  ui->_current_widget = &txt->base;
  return txt;
}

lf_text_t*
lf_text_h3(lf_ui_state_t* ui, const char* label) {
  lf_text_t* txt = lf_text_create_ex(ui, ui->_last_parent, label, ui->font_h3);
  ui->_current_widget = &txt->base;
  return txt;
}

lf_text_t*
lf_text_h4(lf_ui_state_t* ui, const char* label) {
  lf_text_t* txt = lf_text_create_ex(ui, ui->_last_parent, label, ui->font_h4);
  ui->_current_widget = &txt->base;
  return txt;
}

lf_text_t*
lf_text_h5(lf_ui_state_t* ui, const char* label) {
  lf_text_t* txt = lf_text_create_ex(ui, ui->_last_parent, label, ui->font_h5);
  ui->_current_widget = &txt->base;
  return txt;
}

lf_text_t* 
lf_text_h6(lf_ui_state_t* ui, const char* label) {
  lf_text_t* txt = lf_text_create_ex(ui, ui->_last_parent, label, ui->font_h6);
  ui->_current_widget = &txt->base;
  return txt;
}

lf_widget_t*
lf_crnt(lf_ui_state_t* ui) {
  return ui->_current_widget;
}
