#include "../../include/leif/widgets/image.h"
#include <cglm/types-struct.h>

void _img_render(
  lf_ui_state_t* ui, 
  lf_widget_t* widget
) {
  if(!widget || !ui) return;
  lf_image_t* img = (lf_image_t*)widget;
  printf("Rendering image: %i, %ix%i!\n", img->tex_id, img->width, img->height);
  printf("Position: %fx%f!\n", widget->container.pos.x, widget->container.pos.y); 
  ui->render_texture(ui->render_state, widget->container.pos,
                    widget->props.color, img->tex_id, img->width, img->height);
}

lf_image_t* _img_create(
  lf_ui_state_t* ui,
  lf_widget_t* parent,
  const char* filepath) {
  if(!parent) return NULL;
  lf_image_t* img = (lf_image_t*)malloc(sizeof(lf_image_t));

  img->filepath = strdup(filepath);
  ui->render_load_texture(filepath, &img->tex_id, &img->width, &img->height, 0);
  printf("Loaded image: %s, %i, %i\n", filepath, img->width, img->height);

  img->base = *lf_widget_create(
    WidgetTypeImage,
    LF_SCALE_CONTAINER(img->width, img->height),
    ui->theme->img_props,
    _img_render, 
    NULL,
    NULL 
  );

  img->base.layout_type = LayoutNone;
  lf_widget_add_child(parent, (lf_widget_t*)img);

  return img;
}
lf_image_t* lf_image_create(
    lf_ui_state_t* ui,
    lf_widget_t* parent,
    const char* filepath) {
  return _img_create(ui, parent, filepath);
}

