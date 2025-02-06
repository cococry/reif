#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct lf_widget_t lf_widget_t;
typedef struct lf_ui_state_t lf_ui_state_t;

typedef enum {
  LayoutNone = 0,
  LayoutVertical = 0,
  LayoutHorizontal,
  LayoutResponsiveGrid,
} lf_layout_type_t;

typedef enum {
  AlignCenterHorizontal  = 1 << 0,
  AlignCenterVertical = 1 << 1
} lf_alignment_flag_t;

typedef enum {
  JustifyStart = 0,
  JustifySpaceBetween,
  JustifyEnd
} lf_justify_type_t;

typedef enum {
  SizingFitToParent = 0,
  SizingFitToContent,
  SizingGrow
} lf_sizing_type_t;

void lf_layout_vertical(lf_ui_state_t* ui, lf_widget_t* widget);

void lf_layout_horizontal(lf_ui_state_t* ui, lf_widget_t* widget);

void lf_layout_responsive_grid(lf_ui_state_t* ui, lf_widget_t* widget);

void lf_size_calc_vertical(lf_ui_state_t* ui, lf_widget_t* widget);

void lf_size_calc_horizontal(lf_ui_state_t* ui, lf_widget_t* widget);

void lf_size_calc_responsive_grid(lf_ui_state_t* ui, lf_widget_t* widget);

