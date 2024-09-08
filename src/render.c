#include "../include/leif/render.h"
#include "../include/leif/widget.h"
#include "../include/leif/ui_core.h"

#ifdef LF_RUNARA
#include <runara/runara.h>
#include <GL/gl.h>

RnColor _lf_color_to_rn(lf_color_t color) {
  return (RnColor){
    color.r, color.g, color.b, color.a
  };
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
    size, 
    _lf_color_to_rn(color), 
    _lf_color_to_rn(border_color),
    border_width, corner_radius);
}

lf_text_dimension_t lf_rn_render_text(
  void* render_state,
  const char* text,
  void* font,
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
  void* font) {

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

  glEnable(GL_SCISSOR_TEST);

  int32_t y_upper_left = render_height - (area.pos.y + area.size.y);

  glScissor(area.pos.x, y_upper_left, area.size.x, area.size.y);
  
  lf_rn_render_clear_color(color);
    
  glDisable(GL_SCISSOR_TEST);
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

#endif 

void 
lf_render_container(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(!ui->render_rect) {
    fprintf(stderr, "leif: no rectangle rendering function given.\n");
    return;
  }
  ui->render_rect(ui->render_state, 
                  widget->container.pos, 
                  widget->container.size, 
                  widget->props.color, LF_NO_COLOR, 0.0f, 0.0f);
}
