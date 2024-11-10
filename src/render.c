#include "../include/leif/render.h"

#ifdef LF_RUNARA
#include <runara/runara.h>
#include <GL/gl.h>

RnColor _lf_color_to_rn(lf_color_t color) {
  RnColor rn_color =(RnColor){
    color.r, color.g, color.b, color.a 
  };
  return rn_color;
}

void
lf_rn_render_rect(
  void* render_state, 
  vec2s pos,
  vec2s size, 
  lf_color_t color, 
  lf_color_t border_color,
  float border_width,
  float corner_radius) {

  rn_rect_render_ex(
    (RnState*)render_state, pos,
    size, 0.0f,
    _lf_color_to_rn(color),
    _lf_color_to_rn(border_color),
    border_width, corner_radius);
}

lf_text_dimension_t lf_rn_render_text(
  void* render_state,
  const char* text,
  lf_font_t font,
  vec2s pos,
  lf_color_t color) {

  RnTextProps props = rn_text_render(
    (RnState*)render_state, 
    text,
    (RnFont*)font, 
    pos, 
    _lf_color_to_rn(color));

  return (lf_text_dimension_t){
    .width  = props.width,
    .height = props.height
  };
}

lf_text_dimension_t lf_rn_render_get_text_dimension(
  void* render_state,
  const char* text,
  lf_font_t font) {

  RnTextProps props = rn_text_props(
    (RnState*)render_state, 
    text, 
    (RnFont*)font);

  return (lf_text_dimension_t){
    .width  = props.width,
    .height = props.height
  };
}

void 
lf_rn_render_clear_color(
    lf_color_t color) {

  rn_clear_color(_lf_color_to_rn(color));
}

void 
lf_rn_render_clear_color_area(
  lf_color_t color, 
  lf_container_t area, 
  uint32_t render_height) {
  int32_t y_upper_left = render_height - (area.pos.y + area.size.y);
  glEnable(GL_SCISSOR_TEST);
  glScissor(area.pos.x, y_upper_left, area.size.x, area.size.y);
  lf_rn_render_clear_color(color);
}

void 
lf_rn_render_begin(
    void* render_state) {
  rn_begin((RnState*)render_state);
}

void 
lf_rn_render_end(
    void* render_state) {
  rn_end((RnState*)render_state);
}

void 
lf_rn_render_resize_display(
    void* render_state,
    uint32_t width,
    uint32_t height
    ) {
  rn_resize_display((RnState*)render_state, width, height);
}

lf_font_t
lf_rn_render_font_create(
    void* render_state, 
    const char* filepath, 
    uint32_t size) {

  return (lf_font_t)rn_load_font((RnState*)render_state, filepath, size);
}

void
lf_rn_render_font_destroy(
    void* render_state, 
    lf_font_t font) {

  rn_free_font((RnState*)render_state, (RnFont*)font);
}

const char*
lf_rn_render_font_file_from_name(const char* name) {
  return rn_font_file_from_name(name);
}

uint32_t 
lf_rn_render_font_get_size(lf_font_t font) {
  return ((RnFont*)font)->size;
}

#endif 
