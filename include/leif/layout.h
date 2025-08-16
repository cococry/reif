#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct lf_widget_t lf_widget_t;
typedef struct lf_ui_state_t lf_ui_state_t;

typedef enum {
  LF_LAYOUT_NONE = 0,
  LF_LAYOUT_VERTICAL,
  LF_LAYOUT_HORIZONTAL,
  LF_LAYOUT_GRID,
} lf_layout_type_t;

typedef enum {
  LF_ALIGN_CENTER_HORIZONTAL = 1 << 0,
  LF_ALIGN_CENTER_VERTICAL = 1 << 1
} lf_alignment_flag_t;

typedef enum {
  LF_JUSTIFY_START = 0,
  LF_JUSTIFY_SPACE_BETWEEN,
  LF_JUSTIFY_END, 
} lf_justify_type_t;

typedef enum {
  LF_SIZING_NONE = 0,
  LF_SIZING_FIT_PARENT,
  LF_SIZING_FIT_CONTENT,
  LF_SIZING_GROW 
} lf_sizing_type_t;

void lf_layout_vertical(lf_ui_state_t* ui, lf_widget_t* widget);

void lf_layout_horizontal(lf_ui_state_t* ui, lf_widget_t* widget);

void lf_layout_responsive_grid(lf_ui_state_t* ui, lf_widget_t* widget);

void lf_size_calc_vertical(lf_ui_state_t* ui, lf_widget_t* widget);

void lf_size_calc_horizontal(lf_ui_state_t* ui, lf_widget_t* widget);

void lf_size_calc_responsive_grid(lf_ui_state_t* ui, lf_widget_t* widget);

