#include "../include/leif/ui_core.h"
#include "../include/leif/widget.h"
#include "../include/leif/render.h"

#ifdef LF_RUNARA
#include <runara/runara.h>
#endif

#ifdef LF_X11
#include <GL/glx.h>
#endif

lf_ui_state_t*
lf_ui_core_init(lf_window_t* win) {
  lf_ui_state_t* state = malloc(sizeof(*state));


#ifdef LF_RUNARA
#ifdef LF_X11
  state->render_state = rn_init(
    win->width, win->height,
    (RnGLLoader)glXGetProcAddressARB);
#else 
#error 
#error "Invalid windowing system specified (valid windowing systems: LF_X11)"
#endif

  state->render_rect                = lf_rn_render_rect;
  state->render_text                = lf_rn_render_text;
  state->render_get_text_dimension  = lf_rn_render_get_text_dimension;
  state->render_clear_color         = lf_rn_render_clear_color;
  state->render_clear_color_area    = lf_rn_render_clear_color_area;
  state->render_begin               = lf_rn_render_begin;
  state->render_end                 = lf_rn_render_end;
  state->render_resize_display      = lf_rn_render_resize_display;
#endif

  state->win = win;

  state->theme = lf_ui_core_default_theme();

  state->root = lf_widget_container_create(
    NULL, 
    LF_SCALE_CONTAINER(win->width, win->height), 
    LF_NO_COLOR);


  return state;
}

lf_theme_t* 
lf_ui_core_default_theme() {
  lf_theme_t* theme = malloc(sizeof(*theme));

  const uint32_t global_padding = 10;
  const lf_color_t global_color = (lf_color_t){
    .r = 25,
    .g = 25,
    .b = 25,
    .a = 255
  };

  theme->div_props = (lf_widget_props_t){
    .color = LF_NO_COLOR,
    .padding_left = global_padding,
    .padding_right = global_padding,
    .padding_top = global_padding,
    .padding_bottom = global_padding,
  };

  theme->button_props = (lf_widget_props_t){
    .color = global_color, 
    .padding_left = global_padding,
    .padding_right = global_padding,
    .padding_top = global_padding,
    .padding_bottom = global_padding,
  };

  return theme;
}
void 
lf_ui_core_set_theme(
  lf_ui_state_t* ui,
  lf_theme_t* theme) {
  if(ui->theme) {
    free(ui->theme);
  }
  ui->theme = theme;
}

lf_ui_state_t* 
lf_ui_core_init_ex(
    lf_window_t* win,
    lf_render_rect_func_t render_rect,
    lf_render_text_func_t render_text,
    lf_render_get_text_dimension_func_t render_get_text_dimension,
    lf_render_clear_color_func_t render_clear_color,
    lf_render_clear_color_area_func_t render_clear_color_area,
    lf_render_begin_func_t render_begin,
    lf_render_end_func_t render_end,
    lf_render_resize_display_func_t render_resize_display) {

  lf_ui_state_t* state = malloc(sizeof(*state));

  state->win = win;

  state->render_rect                = render_rect;
  state->render_text                = render_text;
  state->render_get_text_dimension  = render_get_text_dimension;
  state->render_clear_color         = render_clear_color;
  state->render_clear_color_area    = render_clear_color_area;
  state->render_begin               = render_begin;
  state->render_end                 = render_end;
  state->render_resize_display      = render_resize_display;

  state->theme = lf_ui_core_default_theme();

  state->root = lf_widget_container_create(
    NULL, 
    LF_SCALE_CONTAINER(win->width, win->height), 
    LF_NO_COLOR);

  return state;
}


bool
lf_ui_core_next_event(lf_ui_state_t* ui) {
  lf_event_t ev = lf_win_next_event(ui->win);
  if(ev.type == WinEventClose) {
    return false;
  }
  if(ev.type == WinEventExpose) {
    lf_ui_core_begin_render(ui, ev.width, ev.height);
    lf_widget_render(ui, ui->root);
    lf_ui_core_end_render(ui);
  }

  return true;
}


void 
lf_ui_core_begin_render(
    lf_ui_state_t* ui, 
    uint32_t render_width,
    uint32_t render_height) {

  ui->root->container = LF_SCALE_CONTAINER(render_width, render_height);

  ui->render_clear_color_area(
    (lf_color_t){0, 0, 0, 0}, 
    ui->root->container, 
  ui->win->height);

  ui->render_resize_display(ui->render_state, render_width, render_height);

  ui->render_begin(ui->render_state);
}

void lf_ui_core_end_render(lf_ui_state_t* ui) {
  ui->render_end(ui->render_state);
  lf_win_swap_buffers(ui->win);
}

void 
lf_ui_core_terminate(lf_ui_state_t* ui) {
  if(!ui) return;

  lf_widget_destroy(ui->root);

  lf_win_destroy(ui->win);

  lf_windowing_terminate();

#ifdef LF_RUNARA
  rn_terminate((RnState*)ui->render_state);
#endif
  
  free(ui);
}
