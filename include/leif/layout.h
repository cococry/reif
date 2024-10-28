#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct lf_widget_t lf_widget_t;
typedef struct lf_ui_state_t lf_ui_state_t;

typedef enum {
  LayoutNone = 0,
  LayoutVertical = 0,
  LayoutHorizontal,
  LayoutGrid,
  LayoutResponsiveGrid
} lf_layout_type_t;

typedef enum {
  AlignCenterHorizontal  = 1 << 0,
  AlignCenterVertical = 1 << 1
} lf_alignment_flag_t;

void lf_layout_vertical(lf_widget_t* widget);

void lf_layout_horizontal(lf_widget_t* widget);

void lf_layout_grid(lf_ui_state_t* ui, lf_widget_t* widget);

void lf_layout_responsive_grid(lf_ui_state_t* ui, lf_widget_t* widget);

void lf_alignment_flag_set(
    uint32_t* flags, 
    lf_alignment_flag_t flag
    );

void lf_alignment_flag_unset(
    uint32_t* flags,
    lf_alignment_flag_t flag
    );

bool lf_alignment_flag_exists(
    uint32_t* flags,
    lf_alignment_flag_t flag
    );
