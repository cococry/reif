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
    .y = lf_widget_height(widget)})                           \

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

typedef struct {
  vec2s pos, size;
} lf_container_t;

#ifdef LF_RUNARA
typedef RnFont* lf_font_t;
#endif

typedef struct {
  lf_color_t color;
  lf_color_t text_color;

  float padding_left, padding_right,
        padding_top, padding_bottom;

  float margin_left, margin_right, 
        margin_top, margin_bottom;

  float corner_radius, border_width;

  lf_color_t border_color;

} lf_widget_props_t;

bool lf_point_intersets_container(vec2s point, lf_container_t container);

bool lf_container_intersets_container(lf_container_t a, lf_container_t b);

float lf_container_area(lf_container_t container);

lf_font_t lf_load_font(lf_ui_state_t* ui, const char* filepath, uint32_t size);

lf_font_t lf_load_font_from_name(lf_ui_state_t* ui, const char* font_name, uint32_t size);

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

