#pragma once 

typedef struct lf_widget_t lf_widget_t;

typedef enum {
  LayoutNone = 0,
  LayoutVertical = 0,
  LayoutHorizontal,
  LayoutCenter
} lf_layout_type_t; 

void lf_layout_vertical(lf_widget_t* widget);

void lf_layout_horizontal(lf_widget_t* widget);

void lf_layout_center(lf_widget_t* widget);
