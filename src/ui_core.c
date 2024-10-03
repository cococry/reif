#include "../include/leif/ui_core.h"
#include "../include/leif/widget.h"
#include "../include/leif/render.h"
#include "../include/leif/util.h"

#include <GL/gl.h>
#include <time.h>

static void init_fonts(lf_ui_state_t* ui);

void 
init_fonts(lf_ui_state_t* ui) {
  const char* fontfile = ui->render_font_file_from_name("Inter");

  ui->font_h1 = ui->render_font_create(ui->render_state, fontfile, 100);
  ui->font_h2 = ui->render_font_create(ui->render_state, fontfile, 24);
  ui->font_h3 = ui->render_font_create(ui->render_state, fontfile, 18);
  ui->font_h4 = ui->render_font_create(ui->render_state, fontfile, 16);
  ui->font_h5 = ui->render_font_create(ui->render_state, fontfile, 13);
  ui->font_h6 = ui->render_font_create(ui->render_state, fontfile, 10);
  ui->font_p = ui->render_font_create(ui->render_state,  fontfile, 16);
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

  state->root = lf_widget_create(
    WidgetTypeUndefined,
    LF_SCALE_CONTAINER(win->width, win->height),
    (lf_widget_props_t){0},
    NULL, NULL, NULL);
 
  return state;
}

lf_theme_t* 
lf_ui_core_default_theme(void) {
  lf_theme_t* theme = malloc(sizeof(*theme));

  const uint32_t global_padding = 10;
  const lf_color_t global_color = (lf_color_t){
    .r = 25,
    .g = 25,
    .b = 25,
    .a = 255
  };
  const lf_color_t lighter_color = (lf_color_t){
    .r = 100,
    .g = 100,
    .b = 100,
    .a = 255
  };

  theme->div_props = (lf_widget_props_t){
    .color = global_color,
    .padding_left = global_padding,
    .padding_right = global_padding,
    .padding_top = global_padding,
    .padding_bottom = global_padding,
    .corner_radius = 0.0, 
    .border_width = 0.0,
    .border_color = lf_color_from_hex(0),
  };

  theme->button_props = (lf_widget_props_t){
    .color = lighter_color, 
    .padding_left = global_padding,
    .padding_right = global_padding,
    .padding_top = global_padding,
    .padding_bottom = global_padding,
    .corner_radius = 0.0, 
    .border_width = 1.0,
    .border_color = lf_color_from_hex(0xffffff)
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
    WidgetTypeUndefined,
    LF_SCALE_CONTAINER(win->win, win->height),
    (lf_widget_props_t){0},
    NULL, NULL, NULL);

  return state;
}


bool
lf_ui_core_next_event(lf_ui_state_t* ui) {
  lf_event_t ev = lf_win_next_event(ui->win);

  if(ev.type == WinEventClose) {
    return false;
  }

  if(ev.type == WinEventExpose) {
    ///Make the root widget rerender if the window size changed 
    if(ui->root->container.size.x != ev.width || 
      ui->root->container.size.y != ev.height) {
      ui->root->needs_rerender = true;
    }

    // Begin a rendering context 
    lf_ui_core_begin_render(
      ui, ev.width, ev.height, 
      (lf_container_t){
        .pos = (vec2s){.x = 0, .y = 0},
        .size = (vec2s){.x = ev.width, .y = ev.height}
      });

    // Rerender the root widget if needs rerender
    if(ui->root->needs_rerender) {
      lf_widget_shape(ui, ui->root);
      lf_widget_render(ui, ui->root);
      ui->root->needs_rerender = false;
    }
    // End the rendering context
    lf_ui_core_end_render(ui);
  }

  lf_widget_handle_event(ui, ui->root, ev);

  // Check if there is some widget to be rerendered
  if(lf_ui_core_needs_rerender(ui, ui->root) && ev.type != WinEventExpose) {
    // Get the area of the largest widget that needs to be rerenderd, so 
    // that we only clear the color buffer where the widget actually is. 
    // This let's us rerender on the widgets that changed instead of the entire 
    // tree.
    lf_container_t max_area = lf_ui_core_get_max_render_area(ui, ui->root);
    lf_ui_core_begin_render(ui, ui->win->width, ui->win->height, max_area); 
    for(uint32_t i = 0; i < ui->root->num_childs; i++) {
      lf_widget_t* child = ui->root->childs[i];
      if(!child->needs_rerender) continue;
      lf_widget_render(ui, child); 
    }
    lf_ui_core_end_render(ui);
  }
  return true;
}

bool
lf_ui_core_needs_rerender(
  lf_ui_state_t* ui, 
  lf_widget_t* widget) {
  if(widget->needs_rerender) {
    return true;
  }
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    if(lf_ui_core_needs_rerender(ui, widget->childs[i])) return true;
  }
  return false;
}

lf_container_t 
lf_ui_core_get_max_render_area(
    lf_ui_state_t* ui,
    lf_widget_t* widget) {
  vec2s min_pos = widget->container.pos;
  vec2s max_size = widget->container.size;
  if(!widget->needs_rerender) {
    min_pos = (vec2s){.x = ui->win->width, .y = ui->win->height};
    max_size = (vec2s){.x =  0, .y = 0};
  }
  for (uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];  
    if(!child->needs_rerender) continue;

    lf_container_t child_max = lf_ui_core_get_max_render_area(ui, child);

    if(child_max.pos.x < min_pos.x ||
       child_max.pos.y < min_pos.y) {
      min_pos = child_max.pos;
    }

    if(lf_container_area(child_max) > 
      lf_container_area(LF_SCALE_CONTAINER(max_size.x, max_size.y))) {
      max_size = child_max.size;
    }
  }
  return (lf_container_t){
    .pos = min_pos,
    .size = max_size
  };
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
  
  free(ui);
}
