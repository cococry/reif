#include "../include/leif/ui_core.h"
#include "../include/leif/widget.h"
#include "../include/leif/render.h"
#include "../include/leif/util.h"

#include <cglm/types-struct.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#ifdef LF_GLFW
#include <GLFW/glfw3.h>
#endif
#ifdef LF_RUNARA
#include <runara/runara.h>
#endif

#define OVERDRAW_CORNER_RADIUS 2 

static void init_fonts(lf_ui_state_t* ui);

static void render_widget_and_submit(
  lf_ui_state_t* ui, 
  lf_widget_t* widget,
  lf_container_t clear_area); 

static void root_shape(lf_ui_state_t* ui, lf_widget_t* widget);
static void win_close_callback(lf_ui_state_t* ui, void* window);
static void win_refresh_callback(lf_ui_state_t* ui, void* window);
static void commit_entire_render(lf_ui_state_t* ui);

void 
win_close_callback(lf_ui_state_t* ui, void* window) {
  (void)window;
  ui->running = false;
}

void 
win_refresh_callback(lf_ui_state_t* ui, void* window) {
  (void)window;
  commit_entire_render(ui);
}

void
commit_entire_render(lf_ui_state_t* ui) {
  vec2s win_size = lf_win_get_size(ui->win);
  lf_container_t clear_area = LF_SCALE_CONTAINER(
    win_size.x,
    win_size.y);
  ui->root->container = clear_area;
  ui->render_resize_display(ui->render_state, win_size.x, win_size.y);
  lf_widget_shape(ui, ui->root);
  render_widget_and_submit(ui, ui->root, clear_area);
  ui->root_needs_render = false;
  lf_win_swap_buffers(ui->win);
}

void 
init_fonts(lf_ui_state_t* ui) {
  const char* fontfile = ui->render_font_file_from_name("Inter");;

  ui->font_h1 = ui->render_font_create(ui->render_state, fontfile, 36);
  ui->font_h2 = ui->render_font_create(ui->render_state, fontfile, 24);
  ui->font_h3 = ui->render_font_create(ui->render_state, fontfile, 18);
  ui->font_h4 = ui->render_font_create(ui->render_state, fontfile, 16);
  ui->font_h5 = ui->render_font_create(ui->render_state, fontfile, 13);
  ui->font_h6 = ui->render_font_create(ui->render_state, fontfile, 10);
  ui->font_p  = ui->render_font_create(ui->render_state, fontfile, 16);
}

void 
render_widget_and_submit(
  lf_ui_state_t* ui, 
  lf_widget_t* widget, 
  lf_container_t clear_area) {

  float overdraw = (widget->props.corner_radius != 0) ? OVERDRAW_CORNER_RADIUS : 0;
  vec2s win_size = lf_win_get_size(ui->win);
  lf_color_t clear_color = 
    (widget->parent) ? 
    widget->parent->props.color : ui->root->props.color;

  lf_ui_core_begin_render(
    ui,
    clear_color,
    win_size.x, win_size.y,
    (lf_container_t){
      .pos = (vec2s){
        .x = clear_area.pos.x - overdraw,
        .y = clear_area.pos.y - overdraw},
      .size = (vec2s){
        .x = clear_area.size.x + widget->props.padding_left + widget->props.padding_right + overdraw * 2,
        .y = clear_area.size.y + widget->props.padding_top + widget->props.padding_bottom + overdraw * 2, 
      }});

  lf_widget_render(ui, widget);
  lf_ui_core_end_render(ui);
}

void 
root_shape(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(!widget) return;
  if(widget->type != WidgetTypeRoot) return;
  lf_widget_apply_layout(ui->root);
}

lf_window_t*
lf_ui_core_create_window(
  uint32_t width, 
  uint32_t height, 
  const char* title) {
  lf_window_t* win = lf_win_create(width, height, title);

  lf_win_set_close_cb(win, win_close_callback);
  lf_win_set_refresh_cb(win, win_refresh_callback);

  lf_win_make_gl_context(win);

  return win;
}

lf_ui_state_t*
lf_ui_core_init(lf_window_t* win) {
  lf_ui_state_t* state = malloc(sizeof(*state));
#ifdef LF_RUNARA
#ifdef LF_X11
  state->render_state = rn_init(
    win->width, win->height,
    (RnGLLoader)glXGetProcAddressARB);
#elif defined LF_GLFW
  state->render_state = rn_init(
    lf_win_get_size(win).x, lf_win_get_size(win).y,
    (RnGLLoader)glfwGetProcAddress);
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
  state->render_font_create         = lf_rn_render_font_create;
  state->render_font_destroy        = lf_rn_render_font_destroy;
  state->render_font_file_from_name = lf_rn_render_font_file_from_name;
  state->render_font_get_size       = lf_rn_render_font_get_size;

#endif

  state->win = win;
  state->refresh_rate = lf_win_get_refresh_rate(win);
  state->_frame_duration = 1.0f / state->refresh_rate;

  state->theme = lf_ui_core_default_theme();


  init_fonts(state);

  state->root = lf_widget_create(
    WidgetTypeRoot,
    LF_SCALE_CONTAINER(lf_win_get_size(win).x, lf_win_get_size(win).y),
    (lf_widget_props_t){0},
    NULL, NULL, root_shape);

  lf_windowing_set_ui_state(state);

  state->root->props.color = state->theme->background_color;
  state->root->layout_type = LayoutVertical;
  state->root->_fixed_width = true;
  state->root->_fixed_height = true;

  state->running = true;
  
  state->_root_never_shaped = true;
  
  state->_last_parent = state->root;
  state->_current_widget = state->root;

  return state;
}

lf_theme_t* 
lf_ui_core_default_theme(void) {
  lf_theme_t* theme = malloc(sizeof(*theme));

  const uint32_t global_padding = 10;
  const uint32_t global_margin = 5;

  theme->text_color = lf_color_from_hex(0xffffff);
  theme->background_color = lf_color_from_hex(0x111111);

  theme->div_props = (lf_widget_props_t){
    .color = LF_NO_COLOR,
    .text_color = theme->text_color,
    .padding_left = global_padding,
    .padding_right = global_padding,
    .padding_top = global_padding,
    .padding_bottom = global_padding,
    .margin_left = 0,
    .margin_right = 0,
    .margin_top = 0,
    .margin_bottom = 0,
    .corner_radius = 0.0f, 
    .border_width = 0.0f, 
    .border_color = LF_NO_COLOR, 
  };

  theme->button_props = (lf_widget_props_t){
    .color = lf_color_from_hex(0xffffff),
    .text_color = lf_color_from_hex(0x111111),
    .padding_left = global_padding,
    .padding_right = global_padding,
    .padding_top = global_padding,
    .padding_bottom = global_padding,
    .margin_left = global_margin,
    .margin_right = global_margin,
    .margin_top = global_margin,
    .margin_bottom = global_margin,
    .corner_radius = 0.0f, 
    .border_width = 0.0f, 
    .border_color = LF_NO_COLOR,
  };

  theme->text_props = (lf_widget_props_t){
    .color = LF_NO_COLOR, 
    .text_color = theme->text_color,
    .padding_left = 0,
    .padding_right = 0,
    .padding_top = 0,
    .padding_bottom = 0,
    .margin_left = global_margin,
    .margin_right = global_margin,
    .margin_top = global_margin,
    .margin_bottom = global_margin,
    .corner_radius = 0.0f, 
    .border_width = 0.0f, 
    .border_color = LF_NO_COLOR,
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
  void* render_state,
  lf_render_rect_func_t render_rect,
  lf_render_text_func_t render_text,
  lf_render_get_text_dimension_func_t render_get_text_dimension,
  lf_render_clear_color_func_t render_clear_color,
  lf_render_clear_color_area_func_t render_clear_color_area,
  lf_render_begin_func_t render_begin,
  lf_render_end_func_t render_end,
  lf_render_resize_display_func_t render_resize_display,
  lf_render_font_create render_font_create,
  lf_render_font_destroy render_font_destroy,
  lf_render_font_file_from_name render_font_file_from_name,
  lf_render_font_get_size render_font_get_size) {

  lf_ui_state_t* state = malloc(sizeof(*state));

  state->win = win;
  state->refresh_rate = lf_win_get_refresh_rate(win);
  state->_frame_duration = 1.0f / state->refresh_rate;

  state->render_state = render_state;
  state->render_rect                = render_rect;
  state->render_text                = render_text;
  state->render_get_text_dimension  = render_get_text_dimension;
  state->render_clear_color         = render_clear_color;
  state->render_clear_color_area    = render_clear_color_area;
  state->render_begin               = render_begin;
  state->render_end                 = render_end;
  state->render_resize_display      = render_resize_display;
  state->render_font_create         = render_font_create;
  state->render_font_destroy        = render_font_destroy;
  state->render_font_file_from_name = render_font_file_from_name;
  state->render_font_get_size       = render_font_get_size;

  state->theme = lf_ui_core_default_theme();

  init_fonts(state);

  state->root = lf_widget_create(
    WidgetTypeRoot,
    LF_SCALE_CONTAINER((float)lf_win_get_size(win).x, lf_win_get_size(win).y),
    (lf_widget_props_t){0},
    NULL, NULL, root_shape);

  state->root->type = WidgetTypeRoot;

  state->root->props.color = state->theme->background_color;
  state->root->layout_type = LayoutVertical;
  state->root->_fixed_width = true;
  state->root->_fixed_height = true;

  state->running = true;

  state->_root_never_shaped = true;
  
  state->_last_parent = state->root;
  state->_current_widget = state->root;

  return state;
}


void
lf_ui_core_next_event(lf_ui_state_t* ui) {
  lf_windowing_next_event();

  float cur_time = glfwGetTime();
  ui->delta_time = cur_time - ui->_last_time;
  ui->_last_time = cur_time;

  if(lf_widget_animate(ui, ui->root)) {
    ui->root_needs_render = true;
  }

  bool rendered = lf_windowing_get_current_event() == WinEventRefresh;

  // Check if there is some widget to be rerendered
  if(ui->root_needs_render) { 
    commit_entire_render(ui);
    rendered = true;
  }

  if(!rendered) {
    usleep((ui->_frame_duration) * 1000000);
  } 


  lf_windowing_update();
}


void 
lf_ui_core_submit(lf_ui_state_t* ui) {
  lf_widget_shape(ui, ui->root);
  if(ui->_root_never_shaped) {
    lf_widget_shape(ui, ui->root);
    ui->_root_never_shaped = true;
  }
  ui->root_needs_render = true;
}

void 
lf_ui_core_begin_render(
  lf_ui_state_t* ui,
  lf_color_t clear_color,
  uint32_t render_width,
  uint32_t render_height,
  lf_container_t render_area) {
  ui->root->container = LF_SCALE_CONTAINER(render_width, render_height);

  ui->render_clear_color_area(clear_color, render_area, render_height);

  ui->render_begin(ui->render_state);
}

void lf_ui_core_end_render(lf_ui_state_t* ui) {
#ifdef LF_RUNARA 
  rn_unset_cull_end_x(
    (RnState*)ui->render_state);
  rn_unset_cull_end_y(
    (RnState*)ui->render_state);
#endif
  ui->render_end(ui->render_state);
}

void 
lf_ui_core_terminate(lf_ui_state_t* ui) {
  if(!ui) return;

  lf_widget_remove(ui->root);

  lf_win_destroy(ui->win);

  lf_windowing_terminate();

#ifdef LF_RUNARA
  rn_terminate((RnState*)ui->render_state);
#endif

  free(ui);
}
