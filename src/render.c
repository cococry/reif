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
    .height = props.height, 
    .paragraph_pos = props.paragraph_pos
  };
}

lf_text_dimension_t 
lf_rn_render_text_paragraph(
      void* render_state,
      const char* text,
      lf_font_t font,
      vec2s pos,
      lf_color_t color,
      lf_paragraph_props_t props) {
  RnTextProps text_props = rn_text_render_paragraph(
    (RnState*)render_state, 
    text,
    (RnFont*)font, 
    pos, 
    _lf_color_to_rn(color),
    (RnParagraphProps){
      .wrap = props.wrap,
      .align = (RnParagraphAlignment)props.align
    });

  return (lf_text_dimension_t){
    .width  = text_props.width,
    .height = text_props.height,
    .paragraph_pos = text_props.paragraph_pos
  };

}

lf_text_dimension_t 
lf_rn_render_get_text_dimension(
  void* render_state,
  const char* text,
  lf_font_t font) {

  RnTextProps props = rn_text_props(
    (RnState*)render_state, 
    text, 
    (RnFont*)font);

  return (lf_text_dimension_t){
    .width  = props.width,
    .height = props.height,
    .paragraph_pos = props.paragraph_pos
  };
}

lf_text_dimension_t 
lf_rn_render_get_text_dimension_paragraph(
  void* render_state,
  const char* text,
  vec2s pos,
  lf_font_t font,
  lf_paragraph_props_t props) {

  RnTextProps text_props = rn_text_props_paragraph(
    (RnState*)render_state, 
    text,
    pos,
    (RnFont*)font,
    (RnParagraphProps){
      .wrap = props.wrap,
      .align = (RnParagraphAlignment)props.align
    });

  return (lf_text_dimension_t){
    .width  = text_props.width,
    .height = text_props.height,
    .paragraph_pos = text_props.paragraph_pos
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

void
lf_rn_render_load_texture(
  const char* filepath, 
  uint32_t* o_tex_id, 
  uint32_t* o_tex_width, 
  uint32_t* o_tex_height,
  uint32_t filter) {
  rn_load_texture_base_types(filepath, o_tex_id, o_tex_width, o_tex_height, filter);
}

void 
lf_rn_render_delete_texture(uint32_t tex_id) {
  rn_free_texture(&(RnTexture){.id = tex_id});
}

void lf_rn_render_texture(
  void* render_state,
  vec2s pos, 
  lf_color_t color,
  lf_texture_id tex_id,
  uint32_t tex_width, uint32_t tex_height,
  float corner_radius,
  float border_width,
  lf_color_t border_color) {
rn_image_render_ex(
  render_state,
  pos, 
  0.0f,
  _lf_color_to_rn(color), 
  (RnTexture){.id = tex_id, .width = tex_width, .height = tex_height},
  _lf_color_to_rn(border_color),
  border_width,
  corner_radius);
}
#endif 
