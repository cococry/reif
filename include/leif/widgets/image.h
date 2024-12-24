#pragma once

#include "../widget.h"
#include "../ui_core.h"
#include "../render.h"

typedef struct {
  lf_widget_t base;

  uint32_t width, height;
  char* filepath;

  lf_texture_id tex_id;
} lf_image_t;

lf_image_t* lf_image_create(
    lf_ui_state_t* ui,
    lf_widget_t* parent,
    const char* filepath);

lf_image_t* lf_image_create_ex(
    lf_ui_state_t* ui,
    lf_widget_t* parent,
    const char* filepath,
    uint32_t w, uint32_t h);
