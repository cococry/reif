#include "../../include/leif/widgets/image.h"
#include <cglm/types-struct.h>

void _img_render(
  lf_ui_state_t* ui, 
  lf_widget_t* widget
) {
  if(!widget || !ui) return;
  lf_image_t* img = (lf_image_t*)widget;
  ui->render_texture(ui->render_state, widget->container.pos,
                    LF_WHITE, img->tex_id, img->width, img->height,
                     widget->props.corner_radius,
                     widget->props.border_width,
                     widget->props.border_color);
}

lf_image_t* _img_create(
  lf_ui_state_t* ui,
  lf_widget_t* parent,
  const char* filepath,
  uint32_t w, uint32_t h) {
  if(!parent) return NULL;
  lf_image_t* img = (lf_image_t*)malloc(sizeof(lf_image_t));

  img->filepath = strdup(filepath);
  lf_mapped_texture_t tex = lf_asset_manager_request_texture(ui, img->filepath);

  img->tex_id = tex.id;
  img->width = tex.width;
  img->height = tex.height;

  if(w != 0 && h != 0) {
    img->width = w;
    img->height = h;
  }

  img->base = *lf_widget_create(
    ui->crnt_widget_id++,
    LF_WIDGET_TYPE_IMAGE,
    LF_SCALE_CONTAINER(img->width, img->height),
    ui->theme->img_props,
    _img_render, 
    NULL,
    NULL,
    NULL
  );

  img->base.layout_type = LF_LAYOUT_NONE;
  lf_widget_add_child(parent, (lf_widget_t*)img);

  return img;
}
lf_image_t* lf_image_create(
    lf_ui_state_t* ui,
    lf_widget_t* parent,
    const char* filepath) {
  return _img_create(ui, parent, filepath, 0, 0);
}

lf_image_t* lf_image_create_ex(
    lf_ui_state_t* ui,
    lf_widget_t* parent,
    const char* filepath,
    uint32_t w, uint32_t h) {
  return _img_create(ui, parent, filepath, w, h);
}

lf_image_t* lf_image_create_ex_w(
    lf_ui_state_t* ui,
    lf_widget_t* parent,
    const char* filepath,
    uint32_t w) {
  if(!parent) return NULL;
  lf_image_t* img = (lf_image_t*)malloc(sizeof(lf_image_t));

  img->filepath = strdup(filepath);
  lf_mapped_texture_t tex = lf_asset_manager_request_texture(ui, img->filepath);
  img->tex_id = tex.id;
  
  img->width = w; 
  img->height = (w * tex.height) / (float)tex.width; 

  img->base = *lf_widget_create(
    ui->crnt_widget_id++,
    LF_WIDGET_TYPE_IMAGE,
    LF_SCALE_CONTAINER(img->width, img->height),
    ui->theme->img_props,
    _img_render, 
    NULL,
    NULL,
    NULL
  );

  img->base.layout_type = LF_LAYOUT_NONE;
  lf_widget_add_child(parent, (lf_widget_t*)img);

  return img;
}


lf_image_t* lf_image_create_ex_h(
    lf_ui_state_t* ui,
    lf_widget_t* parent,
    const char* filepath,
    uint32_t h) {
  if(!parent) return NULL;
  lf_image_t* img = (lf_image_t*)malloc(sizeof(lf_image_t));

  img->filepath = strdup(filepath);
  lf_mapped_texture_t tex = lf_asset_manager_request_texture(ui, img->filepath);
  img->tex_id = tex.id;
  
  img->width = (h * tex.width) / tex.height;
  img->height = h;

  img->base = *lf_widget_create(
    ui->crnt_widget_id++,
    LF_WIDGET_TYPE_IMAGE,
    LF_SCALE_CONTAINER(img->width, img->height),
    ui->theme->img_props,
    _img_render, 
    NULL,
    NULL,
    NULL
  );

  img->base.layout_type = LF_LAYOUT_NONE;
  lf_widget_add_child(parent, (lf_widget_t*)img);

  return img;
}
