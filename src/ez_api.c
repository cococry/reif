#include "../include/leif/ez_api.h"

static void _end_widget(lf_ui_state_t* ui);

static uint32_t font_sizes[] = {
  36, 28, 22, 18, 15, 13, 16
};

void 
_end_widget(lf_ui_state_t* ui) {
  if(!ui->_ez.last_parent || !ui->_ez.last_parent->parent) return;
  ui->_ez.last_parent = ui->_ez.last_parent->parent;
}

lf_ez_state_t 
lf_ez_api_init(lf_ui_state_t* ui) {
  return (lf_ez_state_t){
    .current_widget = ui->root, 
    .last_parent = ui->root,
  };
}

lf_div_t*
lf_div(lf_ui_state_t* ui) {
  if(!ui->_ez.last_parent) return NULL;
  lf_div_t* div = lf_div_create(ui, ui->_ez.last_parent);
  ui->_ez.last_parent = &div->base;
  ui->_ez.current_widget = &div->base;
  return div;
}

void 
lf_div_end(lf_ui_state_t* ui) {
  _end_widget(ui);
}

lf_button_t*
lf_button(lf_ui_state_t* ui) {
  lf_button_t* btn = lf_button_create(ui, ui->_ez.last_parent);
  ui->_ez.last_parent = &btn->base;
  ui->_ez.current_widget = &btn->base;
  return btn;
}

void
lf_button_end(lf_ui_state_t* ui) {
  _end_widget(ui);
}

lf_text_t* 
lf_text_p(lf_ui_state_t* ui, const char* label) {
  int32_t font_size =  
    (ui->_ez.last_parent->font_size != -1 || 
    ui->_ez.current_widget->font_size != -1)   ? 
    (ui->_ez.current_widget->font_size != -1   ? 
    ui->_ez.current_widget->font_size          : 
    ui->_ez.last_parent->font_size)            :
    font_sizes[TextLevelParagraph];

  lf_mapped_font_t font = lf_asset_manager_request_font(
    ui, 
    ui->_ez.current_widget->font_family, 
    ui->_ez.current_widget->font_style, 
    font_size); 

  lf_text_t* txt = lf_text_create_ex(ui, ui->_ez.last_parent, label, font);
  ui->_ez.current_widget = &txt->base;
  return txt;
}


lf_text_t* 
lf_text_h1(lf_ui_state_t* ui, const char* label) {
  lf_mapped_font_t font = lf_asset_manager_request_font(
    ui, 
    ui->_ez.current_widget->font_family, 
    ui->_ez.current_widget->font_style, 
    (ui->_ez.last_parent->font_size != -1 || 
    ui->_ez.current_widget->font_size != -1)   ? 
    (ui->_ez.current_widget->font_size != -1   ? 
    ui->_ez.current_widget->font_size          : 
    ui->_ez.last_parent->font_size)            :
    font_sizes[TextLevelH1]);

  lf_text_t* txt = lf_text_create_ex(ui, ui->_ez.last_parent, label, font);
  ui->_ez.current_widget = &txt->base;
  return txt;
}

lf_text_t* 
lf_text_h2(lf_ui_state_t* ui, const char* label) {
  lf_mapped_font_t font = lf_asset_manager_request_font(
    ui, 
    ui->_ez.current_widget->font_family, 
    ui->_ez.current_widget->font_style, 
    (ui->_ez.last_parent->font_size != -1 || 
    ui->_ez.current_widget->font_size != -1)   ? 
    (ui->_ez.current_widget->font_size != -1   ? 
    ui->_ez.current_widget->font_size          : 
    ui->_ez.last_parent->font_size)            :
    font_sizes[TextLevelH2]);

  lf_text_t* txt = lf_text_create_ex(ui, ui->_ez.last_parent, label, font);
  ui->_ez.current_widget = &txt->base;
  return txt;
}

lf_text_t*
lf_text_h3(lf_ui_state_t* ui, const char* label) {
  lf_mapped_font_t font = lf_asset_manager_request_font(
    ui, 
    ui->_ez.current_widget->font_family, 
    ui->_ez.current_widget->font_style, 
    (ui->_ez.last_parent->font_size != -1 || 
    ui->_ez.current_widget->font_size != -1)   ? 
    (ui->_ez.current_widget->font_size != -1   ? 
    ui->_ez.current_widget->font_size          : 
    ui->_ez.last_parent->font_size)            :
    font_sizes[TextLevelH3]);

  lf_text_t* txt = lf_text_create_ex(ui, ui->_ez.last_parent, label, font); 
  ui->_ez.current_widget = &txt->base;
  return txt;
}

lf_text_t*
lf_text_h4(lf_ui_state_t* ui, const char* label) {
  lf_mapped_font_t font = lf_asset_manager_request_font(
    ui, 
    ui->_ez.current_widget->font_family, 
    ui->_ez.current_widget->font_style, 
    (ui->_ez.last_parent->font_size != -1 || 
    ui->_ez.current_widget->font_size != -1)   ? 
    (ui->_ez.current_widget->font_size != -1   ? 
    ui->_ez.current_widget->font_size          : 
    ui->_ez.last_parent->font_size)            :
    font_sizes[TextLevelH4]);

  lf_text_t* txt = lf_text_create_ex(ui, ui->_ez.last_parent, label, font); 
  ui->_ez.current_widget = &txt->base;
  return txt;
}

lf_text_t*
lf_text_h5(lf_ui_state_t* ui, const char* label) {
  lf_mapped_font_t font = lf_asset_manager_request_font(
    ui, 
    ui->_ez.current_widget->font_family, 
    ui->_ez.current_widget->font_style, 
    (ui->_ez.last_parent->font_size != -1 || 
    ui->_ez.current_widget->font_size != -1)   ? 
    (ui->_ez.current_widget->font_size != -1   ? 
    ui->_ez.current_widget->font_size          : 
    ui->_ez.last_parent->font_size)            :
    font_sizes[TextLevelH5]);

  lf_text_t* txt = lf_text_create_ex(ui, ui->_ez.last_parent, label, font); 
  ui->_ez.current_widget = &txt->base;
  return txt;
}

lf_text_t* 
lf_text_h6(lf_ui_state_t* ui, const char* label) {
  lf_mapped_font_t font = lf_asset_manager_request_font(
    ui, 
    ui->_ez.current_widget->font_family, 
    ui->_ez.current_widget->font_style, 
    (ui->_ez.last_parent->font_size != -1 || 
    ui->_ez.current_widget->font_size != -1)   ? 
    (ui->_ez.current_widget->font_size != -1   ? 
    ui->_ez.current_widget->font_size          : 
    ui->_ez.last_parent->font_size)            :
    font_sizes[TextLevelH6]);

  lf_text_t* txt = lf_text_create_ex(ui, ui->_ez.last_parent, label, font); 
  ui->_ez.current_widget = &txt->base;
  return txt;
}

lf_text_t* 
lf_text_sized(lf_ui_state_t* ui, const char* label, uint32_t pixel_size) {
  lf_mapped_font_t font = lf_asset_manager_request_font(
    ui, 
    ui->_ez.current_widget->font_family, 
    ui->_ez.current_widget->font_style, 
    pixel_size);

  lf_text_t* txt = lf_text_create_ex(ui, ui->_ez.last_parent, label, font); 
  ui->_ez.current_widget = &txt->base;
  return txt;
}

lf_text_dimension_t 
lf_text_measure(lf_ui_state_t* ui, const char* text, lf_font_t font) {
  return ui->render_get_text_dimension(ui->render_state, text, font);
}

lf_widget_t*
lf_crnt(lf_ui_state_t* ui) {
  return ui->_ez.current_widget;
}

lf_image_t* 
lf_image(lf_ui_state_t* ui, const char* filepath) {
  lf_image_t* img = lf_image_create(ui, ui->_ez.last_parent, filepath);
  ui->_ez.current_widget = &img->base;
  return img; 
}

lf_image_t* 
lf_image_sized(lf_ui_state_t* ui, const char* filepath, uint32_t w, uint32_t h) {
  lf_image_t* img = lf_image_create_ex(ui, ui->_ez.last_parent, filepath, w, h);
  ui->_ez.current_widget = &img->base;
  return img; 
}

lf_image_t* 
lf_image_sized_w(lf_ui_state_t* ui, const char* filepath, uint32_t w) {
  lf_image_t* img = lf_image_create_ex_w(ui, ui->_ez.last_parent, filepath, w);
  ui->_ez.current_widget = &img->base;
  return img; 
}


lf_image_t* 
lf_image_sized_h(lf_ui_state_t* ui, const char* filepath, uint32_t h) {
  lf_image_t* img = lf_image_create_ex_h(ui, ui->_ez.last_parent, filepath, h);
  ui->_ez.current_widget = &img->base;
  return img; 
}

