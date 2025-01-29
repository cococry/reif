#pragma once

#include <cglm/cglm.h>
#include <cglm/struct.h>

#include "color.h"
#include "util.h"

typedef struct lf_ui_state_t lf_ui_state_t;
typedef struct lf_widget_t lf_widget_t;

typedef unsigned int lf_texture_id;

typedef struct {
  float width, height;

  vec2s paragraph_pos;
} lf_text_dimension_t;


typedef struct {
  float wrap;
  lf_paragraph_alignment_t align;
} lf_paragraph_props_t;

typedef void (*lf_render_rect_func_t)(
      void* render_state,
      vec2s pos,
      vec2s size, 
      lf_color_t color,
      lf_color_t border_color, 
      float border_width, 
      float corner_radius);

typedef lf_text_dimension_t (*lf_render_text_func_t)(
      void* render_state, 
      const char* text,
      lf_font_t font, 
      vec2s pos,
      lf_color_t color);

typedef lf_text_dimension_t (*lf_render_text_paragraph_func_t)(
      void* render_state, 
      const char* paragraph,
      lf_font_t font, 
      vec2s pos, 
      lf_color_t color,
      lf_paragraph_props_t props);

typedef lf_text_dimension_t (*lf_render_get_text_dimension_func_t)(
      void* render_state,
      const char* text, 
      lf_font_t font); 

typedef lf_text_dimension_t (*lf_render_get_paragraph_dimension_func_t)(
      void* render_state,
      const char* text,
      vec2s paragraph_pos,
      lf_font_t font,
      lf_paragraph_props_t props); 

typedef void (*lf_render_clear_color_func_t)(lf_color_t color); 

typedef void (*lf_render_clear_color_area_func_t)(lf_color_t color, lf_container_t area, uint32_t render_height); 

typedef void (*lf_render_begin_func_t)(void* render_state); 

typedef void (*lf_render_end_func_t)(void* render_State); 

typedef void (*lf_render_resize_display_func_t)(
    void* render_state, uint32_t w, uint32_t h);

typedef lf_font_t (*lf_render_font_create)(
    void* render_state, const char* filepath, uint32_t pixel_size);

typedef lf_font_t (*lf_render_font_create_from_face)(
    void* render_state, const char* filepath, uint32_t pixel_size, uint32_t face_idx);

typedef void (*lf_render_font_destroy)(
    void* render_state, lf_font_t font);

typedef uint32_t (*lf_render_font_get_size)(lf_font_t font);

typedef void (*lf_render_load_texture)(
    const char* filepath,
  uint32_t* o_tex_id, 
  uint32_t* o_tex_width, 
  uint32_t* o_tex_height,
  uint32_t filter);

typedef void (*lf_render_delete_texture)(uint32_t tex_id);

typedef void (*lf_render_texture)(
    void* render_state,
    vec2s pos, 
    lf_color_t color,
    lf_texture_id tex_id,
    uint32_t tex_width, uint32_t tex_height,

    float corner_radius,
    float border_width,
    lf_color_t border_color
    );

#ifdef LF_RUNARA

void lf_rn_render_rect(
    void* render_state, 
    vec2s pos,
    vec2s size, 
    lf_color_t color, 
    lf_color_t border_color,
    float border_width,
    float corner_radius);

lf_text_dimension_t lf_rn_render_text(
      void* render_state,
      const char* text,
      lf_font_t font,
      vec2s pos,
      lf_color_t color);

lf_text_dimension_t lf_rn_render_text_paragraph(
      void* render_state,
      const char* text,
      lf_font_t font,
      vec2s pos,
      lf_color_t color,
      lf_paragraph_props_t props);

lf_text_dimension_t lf_rn_render_get_text_dimension(
      void* render_state,
      const char* text,
      lf_font_t font); 

lf_text_dimension_t lf_rn_render_get_text_dimension_paragraph(
      void* render_state,
      const char* text,
      vec2s pos,
      lf_font_t font,
      lf_paragraph_props_t props); 

void lf_rn_render_clear_color(
    lf_color_t color);

void lf_rn_render_clear_color_area(
    lf_color_t color, 
    lf_container_t area,
    uint32_t render_height);

void lf_rn_render_begin(
    void* render_state);

void lf_rn_render_end(
    void* render_state);

void lf_rn_render_resize_display(
    void* render_state,
    uint32_t width,
    uint32_t height
    );

lf_font_t lf_rn_render_font_create(
    void* render_state, 
    const char* filepath, 
    uint32_t size);

lf_font_t lf_rn_render_font_create_from_face(
    void* render_state, 
    const char* filepath, 
    uint32_t size,
    uint32_t face_idx);

void lf_rn_render_font_destroy(
    void* render_state, 
    lf_font_t font);

const char* lf_rn_render_font_file_from_name(const char* name); 

uint32_t lf_rn_render_font_get_size(lf_font_t font); 

void lf_rn_render_load_texture(
  const char* filepath, 
  uint32_t* o_tex_id, 
  uint32_t* o_tex_width, 
  uint32_t* o_tex_height,
  uint32_t filter);

void lf_rn_render_delete_texture(uint32_t tex_id);

void lf_rn_render_texture(
    void* render_state,
    vec2s pos, 
    lf_color_t color,
    lf_texture_id tex_id,
    uint32_t tex_width, uint32_t tex_height,
    float corner_radius,
    float border_width,
    lf_color_t border_color);

#endif 

