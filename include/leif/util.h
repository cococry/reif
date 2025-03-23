#pragma once

#include <cglm/cglm.h>
#include <cglm/struct.h>
#include <stdbool.h>

#include "color.h"

#ifdef LF_RUNARA
#include <runara/runara.h>
#endif

typedef struct lf_ui_state_t lf_ui_state_t;

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) (a) < (b) ? (a) : (b)

#define VEC_INIT_CAP 4

#define LF_SCALE_CONTAINER(width, height) ((lf_container_t){  \
  .pos = (vec2s){.x = 0, .y = 0},                             \
  .size = (vec2s){.x = (width), .y = (height)}}) 

#define LF_WIDGET_SIZE_V2(widget) ((vec2s){                   \
    .x = lf_widget_width((widget)),                           \
    .y = lf_widget_height((widget))})                          \

#define LF_WIDGET_SIZE_RENDERED_V2(widget) ((vec2s){                    \
    .x = lf_widget_width_ex(widget, widget->_rendered_props),         \
    .y = lf_widget_height_ex(widget, widget->_rendered_props)})         \

#define LF_WIDGET_CONTAINER(widget) ((lf_container_t){        \
    .pos = (widget)->container.pos,                           \
    .size = LF_WIDGET_SIZE_V2((widget))})                     \

#define lf_vector_init(vec)                                                       \
do {                                                                              \
  (vec)->items = malloc(sizeof(*(vec)) * VEC_INIT_CAP);                           \
  (vec)->size = 0;                                                                \
  (vec)->cap = 0;                                                                 \
} while (0)

#define lf_vector_free(vec)                                                       \
do {                                                                              \
  if((vec)->items) {                                                              \
    free((vec)->items);                                                           \
    (vec)->items = NULL;                                                          \
  }                                                                               \
  (vec)->size = 0;                                                                \
  (vec)->cap = 0;                                                                 \
} while (0)

#define lf_vector_remove_by_idx(vec, idx)                                         \
do {                                                                              \
  for (uint32_t _i = (idx); _i < (vec)->size - 1; _i++) {                         \
  (vec)->items[_i] = (vec)->items[_i + 1];                                        \
  }                                                                               \
  (vec)->size--;                                                                  \
  (vec)->items = realloc((vec)->items, (vec)->size * sizeof(*(vec)->items));      \
} while (0)                                                                       

#define lf_vector_append(vec, item)                                               \
do {                                                                              \
  if ((vec)->size >= (vec)->cap) {                                                \
    (vec)->cap = (vec)->cap == 0 ? VEC_INIT_CAP : (vec)->cap*2;                   \
    (vec)->items = realloc((vec)->items, (vec)->cap*sizeof(*(vec)->items));       \
  }                                                                               \
  (vec)->items[(vec)->size++] = (item);                                           \
} while (0)

#define lf_clamp(value, min, max) ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))

typedef enum {
  LF_FONT_STYLE_REGULAR, 
  LF_FONT_STYLE_ITALIC,
  LF_FONT_STYLE_OBLIQUE,
  LF_FONT_STYLE_BOLD,
  LF_FONT_STYLE_BOLD_ITALIC,
  LF_FONT_STYLE_SEMI_BOLD,
  LF_FONT_STYLE_SEMI_BOLD_ITALIC,
  LF_FONT_STYLE_LIGHT,
  LF_FONT_STYLE_LIGHT_ITALIC,
  LF_FONT_STYLE_EXTRA_LIGHT,
  LF_FONT_STYLE_EXTRA_LIGHT_ITALIC,
  LF_FONT_STYLE_MEDIUM,
  LF_FONT_STYLE_MEDIUM_ITALIC,
  LF_FONT_STYLE_THIN,
  LF_FONT_STYLE_THIN_ITALIC,
  LF_FONT_STYLE_EXTRA_BOLD,
  LF_FONT_STYLE_EXTRA_BOLD_ITALIC,
  LF_FONT_STYLE_CONDENSED,
  LF_FONT_STYLE_CONDENSED_ITALIC,
  LF_FONT_STYLE_EXPANDED,
  LF_FONT_STYLE_EXPANDED_ITALIC,
  LF_FONT_STYLE_COUNT
} lf_font_style_t;

#define LF_FONT_STYLE_TO_STRING(style) \
    ((style) == LF_FONT_STYLE_REGULAR ? "Regular" : \
    (style) == LF_FONT_STYLE_ITALIC ? "Italic" : \
    (style) == LF_FONT_STYLE_OBLIQUE ? "Oblique" : \
    (style) == LF_FONT_STYLE_BOLD ? "Bold" : \
    (style) == LF_FONT_STYLE_BOLD_ITALIC ? "Bold Italic" : \
    (style) == LF_FONT_STYLE_SEMI_BOLD ? "SemiBold" : \
    (style) == LF_FONT_STYLE_SEMI_BOLD_ITALIC ? "SemiBold Italic" : \
    (style) == LF_FONT_STYLE_LIGHT ? "Light" : \
    (style) == LF_FONT_STYLE_LIGHT_ITALIC ? "Light Italic" : \
    (style) == LF_FONT_STYLE_EXTRA_LIGHT ? "ExtraLight" : \
    (style) == LF_FONT_STYLE_EXTRA_LIGHT_ITALIC ? "ExtraLight Italic" : \
    (style) == LF_FONT_STYLE_MEDIUM ? "Medium" : \
    (style) == LF_FONT_STYLE_MEDIUM_ITALIC ? "Medium Italic" : \
    (style) == LF_FONT_STYLE_THIN ? "Thin" : \
    (style) == LF_FONT_STYLE_THIN_ITALIC ? "Thin Italic" : \
    (style) == LF_FONT_STYLE_EXTRA_BOLD ? "ExtraBold" : \
    (style) == LF_FONT_STYLE_EXTRA_BOLD_ITALIC ? "ExtraBold Italic" : \
    (style) == LF_FONT_STYLE_CONDENSED ? "Condensed" : \
    (style) == LF_FONT_STYLE_CONDENSED_ITALIC ? "Condensed Italic" : \
    (style) == LF_FONT_STYLE_EXPANDED ? "Expanded" : \
    (style) == LF_FONT_STYLE_EXPANDED_ITALIC ? "Expanded Italic" : "Unknown")

typedef struct lf_widget_t lf_widget_t;

typedef struct {
  vec2s pos, size;
} lf_container_t;

#ifdef LF_RUNARA
typedef RnFont* lf_font_t;
#endif

typedef enum {
  ParagraphAlignmentLeft = 0,
  ParagraphAlignmentCenter,
  ParagraphAlignmentRight,
} lf_paragraph_alignment_t;

typedef struct {
  lf_color_t color;
  lf_color_t text_color;

  float padding_left, padding_right,
        padding_top, padding_bottom;

  float margin_left, margin_right, 
        margin_top, margin_bottom;

  float corner_radius, border_width;

  lf_color_t border_color;

  lf_paragraph_alignment_t text_align;
  
  float corner_radius_percent;

} lf_widget_props_t;

typedef void (*lf_component_func_t)(void);

typedef struct {
  lf_component_func_t func;
  uint32_t _child_idx;
  lf_widget_t* _parent;
} lf_component_t;

typedef struct {
  lf_component_t* items;
  uint32_t cap, size;
} lf_component_list_t;


typedef struct {
  lf_widget_t* last_parent, *current_widget;
  bool _assignment_only;

  int32_t index_stack[16];
  int32_t index_depth;

  lf_component_list_t comps;
} lf_ez_state_t;

bool lf_point_intersets_container(vec2s point, lf_container_t container);

bool lf_container_intersets_container(lf_container_t a, lf_container_t b);

float lf_container_area(lf_container_t container);

lf_font_t lf_load_font(lf_ui_state_t* ui, const char* filepath, uint32_t size);

void lf_font_resize(lf_ui_state_t* ui, lf_font_t font, uint32_t size);

uint32_t lf_font_get_size(lf_ui_state_t* ui, lf_font_t font);

float lf_ease_linear(float t);

float lf_ease_out_quad(float t);

float lf_ease_in_quad(float t);

float lf_ease_in_out_quad(float t);

void lf_flag_set(
    uint32_t* flags, 
    uint32_t flag
    );

void lf_flag_unset(
    uint32_t* flags,
    uint32_t flag
    );

bool lf_flag_exists(
    uint32_t* flags,
    uint32_t flag
    );

uint64_t lf_djb2_hash(const unsigned char* str);

