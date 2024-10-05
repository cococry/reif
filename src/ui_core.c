#include "../include/leif/ui_core.h"
#include "../include/leif/widget.h"
#include "../include/leif/render.h"
#include "../include/leif/util.h"

#include <GL/gl.h>
#include <cglm/types-struct.h>
#include <time.h>

#define RENDER_OVERDRAW_FACTOR 8
#define MAX_DIRTY_WIDGETS 10

static void init_fonts(lf_ui_state_t* ui);

static void render_widget_and_submit(
  lf_ui_state_t* ui, 
  lf_widget_t* widget,
  lf_container_t clear_area);

static void root_shape(lf_ui_state_t* ui, lf_widget_t* widget);

void 
init_fonts(lf_ui_state_t* ui) {
  const char* fontfile = ui->render_font_file_from_name("Inter");

  ui->font_h1 = ui->render_font_create(ui->render_state, fontfile, 36);
  ui->font_h2 = ui->render_font_create(ui->render_state, fontfile, 24);
  ui->font_h3 = ui->render_font_create(ui->render_state, fontfile, 18);
  ui->font_h4 = ui->render_font_create(ui->render_state, fontfile, 16);
  ui->font_h5 = ui->render_font_create(ui->render_state, fontfile, 13);
  ui->font_h6 = ui->render_font_create(ui->render_state, fontfile, 10);
  ui->font_p  = ui->render_font_create(ui->render_state, fontfile, 16);
}

void 
render_widget_and_submit(lf_ui_state_t* ui, lf_widget_t* widget, lf_container_t clear_area) {
  lf_ui_core_begin_render(
    ui, 
    ui->win->width, ui->win->height, 
    (lf_container_t){
      .pos = (vec2s){
        .x = clear_area.pos.x - widget->props.corner_radius,
        .y = clear_area.pos.y - widget->props.corner_radius},
      .size = (vec2s){
        .x = clear_area.size.x + RENDER_OVERDRAW_FACTOR, 
        .y = clear_area.size.y + RENDER_OVERDRAW_FACTOR, 
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
  state->render_font_create         = lf_rn_render_font_create;
  state->render_font_destroy        = lf_rn_render_font_destroy;
  state->render_font_file_from_name = lf_rn_render_font_file_from_name;
  state->render_font_get_size       = lf_rn_render_font_get_size;

#endif

  state->win = win;

  state->theme = lf_ui_core_default_theme();

  init_fonts(state);

  state->dirty_widgets = malloc(sizeof(lf_widget_t*) * MAX_DIRTY_WIDGETS);
  state->num_dirty = 0;

  state->root = lf_widget_create(
    WidgetTypeRoot,
    LF_SCALE_CONTAINER(win->width, win->height),
    (lf_widget_props_t){0},
    NULL, NULL, root_shape);
 
  state->root->props.color = state->theme->background_color;
  state->root->layout_type = LayoutVertical;

  return state;
}

lf_theme_t* 
lf_ui_core_default_theme(void) {
  lf_theme_t* theme = malloc(sizeof(*theme));

  const uint32_t global_padding = 0;
  const uint32_t global_margin = 0;


  theme->div_props = (lf_widget_props_t){
    .color = lf_color_from_hex(0xffffff),
    .padding_left = global_padding,
    .padding_right = global_padding,
    .padding_top = global_padding,
    .padding_bottom = global_padding,
    .margin_left = 0,
    .margin_right = 0,
    .margin_top = 0,
    .margin_bottom = 0,
    .corner_radius = 0.0, 
    .border_width = 0.0,
    .border_color = lf_color_from_hex(0),
  };

  theme->button_props = (lf_widget_props_t){
    .color = lf_color_from_hex(0xffffff),
    .padding_left = global_padding,
    .padding_right = global_padding,
    .padding_top = global_padding,
    .padding_bottom = global_padding,
    .margin_left = global_margin,
    .margin_right = global_margin,
    .margin_top = global_margin,
    .margin_bottom = global_margin,
    .corner_radius = 0.0, 
    .border_width = 0.0,
  };

  theme->text_color = lf_color_from_hex(0x333333);
  theme->background_color = lf_color_from_hex(0xeeeeee);

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

  state->dirty_widgets = malloc(sizeof(lf_widget_t*) * MAX_DIRTY_WIDGETS);
  state->num_dirty = 0;

  init_fonts(state);

  state->root = lf_widget_create(
    WidgetTypeRoot,
    LF_SCALE_CONTAINER(win->win, win->height),
    (lf_widget_props_t){0},
    NULL, NULL, root_shape);

  state->root->props.color = state->theme->background_color;
  state->root->layout_type = LayoutVertical;

  return state;
}


bool
lf_ui_core_next_event(lf_ui_state_t* ui) {
  lf_event_t ev = lf_win_next_event(ui->win);

  if(ev.type == WinEventClose) {
    return false;
  }

  if(ev.type == WinEventExpose) {
      lf_container_t clear_area = LF_SCALE_CONTAINER(
        ev.width,
        ev.height); 
    lf_widget_shape(ui, ui->root);
    render_widget_and_submit(ui, ui->root, clear_area);
    ui->root->needs_rerender = false;
    printf("EXPOSE: %i, %i\n", ev.width, ev.height);
  }

  lf_widget_handle_event(ui, ui->root, ev);

  // Check if there is some widget to be rerendered
  if(ui->root->needs_rerender && ev.type != WinEventExpose) {
      lf_container_t clear_area = LF_SCALE_CONTAINER(
        ui->win->width,
        ui->win->height); 
    lf_widget_shape(ui, ui->root);
    render_widget_and_submit(ui, ui->root, clear_area);
    ui->root->needs_rerender = false;
  } else if(ev.type != WinEventExpose) {
    if(ui->num_dirty < MAX_DIRTY_WIDGETS && ui->num_dirty != 0) { 
      lf_ui_core_rerender_dirty(ui);
    } else if(ui->num_dirty != 0){
      lf_container_t clear_area = LF_SCALE_CONTAINER(
        ui->win->width,
        ui->win->height); 
      lf_widget_shape(ui, ui->root);
      render_widget_and_submit(ui, ui->root, clear_area);
      ui->root->needs_rerender = false;
    }
  }
  return true;
}


void lf_ui_core_rerender_dirty(lf_ui_state_t* ui) {
  for(uint32_t i = 0; i < ui->num_dirty; i++) {
    render_widget_and_submit(ui, ui->dirty_widgets[i], ui->dirty_widgets[i]->container);
  }
  memset(ui->dirty_widgets, 0, ui->num_dirty * sizeof(lf_widget_t*));
  ui->num_dirty = 0;
}

void 
lf_ui_core_rerender(lf_ui_state_t* ui) {
  lf_widget_shape(ui, ui->root); 
  ui->root->needs_rerender = true;
}

void 
lf_ui_core_make_dirty(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(ui->num_dirty + 1 > MAX_DIRTY_WIDGETS) return;
  ui->dirty_widgets[ui->num_dirty++] = widget;
  widget->needs_rerender = true;
}

void 
lf_ui_core_begin_render(
    lf_ui_state_t* ui, 
    uint32_t render_width,
    uint32_t render_height,
  lf_container_t render_area) {

  ui->root->container = LF_SCALE_CONTAINER(render_width, render_height);

  ui->render_clear_color_area(ui->root->props.color, render_area, render_height);

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
  
  free(ui->dirty_widgets);
  free(ui);
}
