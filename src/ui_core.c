#include "../include/leif/ui_core.h"
#include "../include/leif/widget.h"
#include "../include/leif/render.h"
#include "../include/leif/util.h"
#include "../include/leif/ez_api.h"

#include <cglm/types-struct.h>
#include <fontconfig/fontconfig.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#ifdef LF_GLFW
#include <GLFW/glfw3.h>
#endif
#ifdef LF_RUNARA
#include <runara/runara.h>
#endif
#ifdef LF_X11 
#include <GL/glx.h>
#endif


typedef struct {
  lf_windowing_hint_kv_t* items;
  uint32_t cap, size;
} lf_windowing_hints_list_t;

#define OVERDRAW_CORNER_RADIUS 2 
#define MAX_CONCURRENT_TIMERS 16


static float get_elapsed_time(void);

static void render_widget_and_submit(
  lf_ui_state_t* ui, 
  lf_widget_t* widget,
  lf_container_t clear_area); 

static void root_shape(lf_ui_state_t* ui, lf_widget_t* widget);
static void root_resize(lf_ui_state_t* ui, lf_widget_t* widget, lf_event_t ev);
static void win_close_callback(lf_ui_state_t* ui, lf_window_t window);
static void win_refresh_callback(lf_ui_state_t* ui, lf_window_t window);
static void remove_marked_widgets(lf_widget_t* root);
static void interrupt_all_animations_recursively(lf_widget_t* widget);
static void default_root_layout_func(lf_ui_state_t* ui);
static void init_state(lf_ui_state_t* state, lf_window_t win);

static uint32_t window_flags = 0;
struct timespec init_time;

static lf_windowing_hints_list_t windowing_hints;

void 
win_close_callback(lf_ui_state_t* ui, lf_window_t window) {
  (void)window;
  ui->running = false;
}

void 
win_refresh_callback(lf_ui_state_t* ui, lf_window_t window) {
  vec2s winsize = lf_win_get_size(window);
  ui->root->container = LF_SCALE_CONTAINER(winsize.x, winsize.y);
  ui->root->_needs_rerender = true;
}

void 
remove_marked_widgets(lf_widget_t* root) {
  if (!root) return;

  for (uint32_t i = 0; i < root->num_childs; ) {
    lf_widget_t* child = root->childs[i];
    if (child->_marked_for_removal) {
      lf_widget_remove_child_from_memory(root, i);
    } else {
      remove_marked_widgets(child);
      i++;
    }
  }

  if (root->_marked_for_removal) {
    lf_widget_remove_from_memory(root);
  }
}

void 
interrupt_all_animations_recursively(lf_widget_t* widget) {
  lf_widget_interrupt_all_animations(widget);

  for(uint32_t i = 0; i < widget->num_childs; i++) {
    interrupt_all_animations_recursively(widget->childs[i]);
  }
}

void 
default_root_layout_func(lf_ui_state_t* ui) {
  lf_ui_core_display_current_page(ui);
}

void 
init_state(lf_ui_state_t* state, lf_window_t win) {
  state->win = win;
  state->refresh_rate = lf_win_get_refresh_rate(win);
  state->_frame_duration = 1.0f / state->refresh_rate;

  state->theme = lf_ui_core_default_theme();

  state->fontpath = NULL;

  lf_vector_init(&state->pages);
  lf_vector_init(&state->timers);
  lf_vector_init(&windowing_hints);

  state->asset_manager = lf_asset_manager_init();

  state->crnt_page_id = 0;
  state->crnt_widget_id = 0;

  lf_ui_core_set_root_layout(state, default_root_layout_func);

  state->root = lf_widget_create(
    state->crnt_widget_id++,
    WidgetTypeRoot,
    LF_SCALE_CONTAINER(lf_win_get_size(win).x, lf_win_get_size(win).y),
    (lf_widget_props_t){0},
    NULL, NULL, root_shape, NULL);

  state->root->font_family = "Inter";
  state->root->font_style = LF_FONT_STYLE_REGULAR;
  state->root->font_size = -1;

  lf_widget_set_listener(state->root, root_resize, WinEventResize);

  lf_windowing_set_ui_state(state);

  state->root->props.color = state->theme->background_color;
  state->root->props.text_color = state->theme->text_color;
  state->root->layout_type = LayoutVertical;
  state->root->_fixed_width = true;
  state->root->_fixed_height = true;

  state->running = true;

  state->_ez = lf_ez_api_init(state); 

  clock_gettime(CLOCK_MONOTONIC, &init_time);
  state->_last_time = get_elapsed_time();
  state->delta_time = 0.0f;

}

float 
get_elapsed_time(void) {
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  float elapsed_time = (now.tv_sec - init_time.tv_sec) +
    (now.tv_nsec - init_time.tv_nsec) / 1e9f;
  return elapsed_time;
}

void 
render_widget_and_submit(
  lf_ui_state_t* ui, 
  lf_widget_t* widget, 
  lf_container_t clear_area) {
  if(!ui || !widget) return;

  float overdraw = (widget->props.corner_radius != 0) ? OVERDRAW_CORNER_RADIUS : 0;
  vec2s win_size = lf_win_get_size(ui->win);
  lf_color_t clear_color = 
    (widget->parent) && (widget->parent->parent) ? 
    widget->parent->parent->props.color : ui->root->props.color;

  lf_ui_core_begin_render(
    ui,
    clear_color,
    win_size.x, win_size.y,
    (lf_container_t){
      .pos = (vec2s){
        .x = clear_area.pos.x - overdraw,
        .y = clear_area.pos.y - overdraw},
      .size = (vec2s){
        .x = clear_area.size.x + widget->props.padding_left +
	widget->props.padding_right + overdraw * 2,
        .y = clear_area.size.y + widget->props.padding_top +
	widget->props.padding_bottom + overdraw * 2, 
      }});

  lf_widget_render(ui, widget);

  lf_ui_core_end_render(ui);
}

void 
root_shape(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(!widget) return;
  if(widget->type != WidgetTypeRoot) return;
  lf_widget_apply_layout(ui, ui->root);
}

void 
root_resize(lf_ui_state_t* ui, lf_widget_t* widget, lf_event_t ev) {
  (void)ev;
  if(!widget) return;
  if(widget->type != WidgetTypeRoot) return;
  ui->root->container = LF_SCALE_CONTAINER(ev.width, ev.height);
  ui->root->_needs_rerender = true;
  lf_widget_shape(ui, ui->root);
}

void 
lf_ui_core_set_window_flags(uint32_t flags) {
  window_flags = flags;
}

void 
lf_ui_core_set_window_hint(lf_window_hint_t hint, uint32_t value) {
  lf_vector_append(&windowing_hints, ((lf_windowing_hint_kv_t)
    {.key = hint, 
      .value = value}));
}

lf_window_t
lf_ui_core_create_window(
  uint32_t width, 
  uint32_t height, 
  const char* title) {
  lf_window_t win = lf_win_create_ex(
    width, height, 
    title, 
    window_flags, 
    windowing_hints.items, windowing_hints.size);

  lf_win_set_close_cb(win, win_close_callback);
  lf_win_set_refresh_cb(win, win_refresh_callback);

  lf_win_make_gl_context(win);

  return win;
}

lf_ui_state_t*
lf_ui_core_init(lf_window_t win) {
  lf_ui_state_t* state = malloc(sizeof(*state));
#ifdef LF_RUNARA
#ifdef LF_X11
  state->render_state = rn_init(
    lf_win_get_size(win).x, lf_win_get_size(win).y,
    (RnGLLoader)glXGetProcAddressARB);
#elif defined LF_GLFW
  state->render_state = rn_init(
    lf_win_get_size(win).x, lf_win_get_size(win).y,
    (RnGLLoader)glfwGetProcAddress);
#else 
  #error 
#error "Invalid windowing system specified (valid windowing systems: LF_GLFW, LF_X11)"
#endif

  state->render_rect                = lf_rn_render_rect;
  state->render_text                = lf_rn_render_text;
  state->render_paragraph           = lf_rn_render_text_paragraph;
  state->render_get_text_dimension  = lf_rn_render_get_text_dimension;
  state->render_get_paragraph_dimension = lf_rn_render_get_text_dimension_paragraph;
  state->render_clear_color         = lf_rn_render_clear_color;
  state->render_clear_color_area    = lf_rn_render_clear_color_area;
  state->render_begin               = lf_rn_render_begin;
  state->render_end                 = lf_rn_render_end;
  state->render_resize_display      = lf_rn_render_resize_display;
  state->render_font_create         = lf_rn_render_font_create;
  state->render_font_create_from_face = lf_rn_render_font_create_from_face;
  state->render_font_destroy        = lf_rn_render_font_destroy;
  state->render_font_get_size       = lf_rn_render_font_get_size;
  state->render_load_texture        = lf_rn_render_load_texture;
  state->render_delete_texture      = lf_rn_render_delete_texture;
  state->render_texture             = lf_rn_render_texture;

#endif

  init_state(state, win);

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
    .text_align = ParagraphAlignmentLeft, 
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
    .text_align = ParagraphAlignmentLeft,
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
    .text_align = ParagraphAlignmentLeft,
  };

  theme->img_props = (lf_widget_props_t){
    .color = LF_WHITE, 
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
    .text_align = ParagraphAlignmentLeft,
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

lf_ui_state_t* lf_ui_core_init_ex(
  lf_window_t win, 
  void* render_state,
  lf_render_rect_func_t render_rect,
  lf_render_text_func_t render_text,
  lf_render_text_paragraph_func_t render_paragraph,
  lf_render_get_text_dimension_func_t render_get_text_dimension,
  lf_render_get_paragraph_dimension_func_t render_get_paragraph_dimension,
  lf_render_clear_color_func_t render_clear_color,
  lf_render_clear_color_area_func_t render_clear_color_area,
  lf_render_begin_func_t render_begin,
  lf_render_end_func_t render_end,
  lf_render_resize_display_func_t render_resize_display,
  lf_render_font_create render_font_create,
  lf_render_font_create_from_face render_font_create_from_face,
  lf_render_font_destroy render_font_destroy,
  lf_render_font_get_size render_font_get_size,
  lf_render_load_texture render_load_texture,
  lf_render_delete_texture render_delete_texture,
  lf_render_texture render_texture) {

  lf_ui_state_t* state = malloc(sizeof(*state));
  state->render_state = render_state;
  state->render_rect                = render_rect;
  state->render_text                = render_text;
  state->render_paragraph           = render_paragraph;
  state->render_get_text_dimension  = render_get_text_dimension;
  state->render_get_paragraph_dimension = render_get_paragraph_dimension; 
  state->render_clear_color         = render_clear_color;
  state->render_clear_color_area    = render_clear_color_area;
  state->render_begin               = render_begin;
  state->render_end                 = render_end;
  state->render_resize_display      = render_resize_display;
  state->render_font_create         = render_font_create;
  state->render_font_create_from_face = render_font_create_from_face;
  state->render_font_destroy        = render_font_destroy;
  state->render_font_get_size       = render_font_get_size;
  state->render_load_texture        = render_load_texture;
  state->render_delete_texture      = render_delete_texture;
  state->render_texture             = render_texture; 

  init_state(state, win);
  return state;
}


bool rerender_dirty_children(lf_ui_state_t* ui, lf_widget_t* widget, bool parent_rendered) {
  if (widget->_needs_rerender) {
    if (parent_rendered) {
      widget->_needs_rerender = false;
      return false;
    }

    lf_container_t clear_area = widget->container; 
    if (widget->_max_size.x != -1.0f && clear_area.size.x > widget->_max_size.x) {
      clear_area.size.x = widget->_max_size.x;
    }
    if (widget->_max_size.y != -1.0f && clear_area.size.y > widget->_max_size.y) {
      clear_area.size.y = widget->_max_size.y;
    }
    
    render_widget_and_submit(ui, widget, clear_area);
    widget->_needs_rerender = false;
    parent_rendered = true;
  }

  bool rendered = parent_rendered;
  for (uint32_t i = 0; i < widget->num_childs; i++) {
    if (rerender_dirty_children(ui, widget->childs[i], parent_rendered)) {
      rendered = true;
    }
  }
  
  return rendered;
}

void
lf_ui_core_next_event(lf_ui_state_t* ui) {
  if(ui->crnt_page_id == 0 && ui->pages.size != 0) {
    lf_ui_core_set_page_by_id(ui, ui->pages.items[0].id);
    fprintf(stderr, "leif: no active page set, but pages available, defaulting to first page.\n");
  }
  lf_windowing_next_event();

  for (uint32_t i = 0; i < ui->timers.size; i++) {
    if(!ui->timers.items[i].paused) {
      lf_timer_tick(ui, &ui->timers.items[i], ui->delta_time, false);
    }
  }

  // Mark expired timers for deletion
  for (uint32_t i = 0; i < ui->timers.size; i++) {
    if (ui->timers.items[i].elapsed >= ui->timers.items[i].duration && !ui->timers.items[i].paused) {
      ui->timers.items[i].expired = true; 
    }
  }

  if(ui->root->_needs_rerender) {
    remove_marked_widgets(ui->root);
  }

  float cur_time = get_elapsed_time();
  ui->delta_time = cur_time - ui->_last_time;
  ui->_last_time = cur_time;

  lf_widget_t* shape = NULL;
  if(lf_widget_animate(ui, ui->root, &shape)) {
    lf_widget_t* widget = shape->parent ? shape->parent : ui->root;
    lf_ui_core_rerender_widget(ui, widget);
  }

  bool rendered = lf_windowing_get_current_event() == WinEventRefresh;

  if(ui->root->_needs_rerender) {
    lf_widget_shape(ui, ui->root);
    lf_ui_core_commit_entire_render(ui);
    ui->root->_needs_rerender = false;
    rendered = true;
  } else {
    rendered = false;
    for (uint32_t i = 0; i < ui->root->num_childs; i++) {
      bool rendered_child = rerender_dirty_children(ui, ui->root->childs[i], false);
      if (rendered_child) {
        rendered = true;
      }
    }
    if(rendered) {
      lf_win_swap_buffers(ui->win);
    }
  }

  if(!rendered) {
    usleep((ui->_frame_duration) * 1000000);
  } 

  lf_windowing_update();

  // Remove expired timers
  for (uint32_t i = 0; i < ui->timers.size;) {
    if (ui->timers.items[i].expired && !ui->timers.items[i].looping && !ui->timers.items[i].paused) {
      lf_vector_remove_by_idx(&ui->timers, i);
    } else {
      i++;
    }
  }

  for (uint32_t i = 0; i < ui->timers.size; i++) {
    if(ui->timers.items[i].expired && ui->timers.items[i].looping && !ui->timers.items[i].paused) {
      ui->timers.items[i].expired = false;
      ui->timers.items[i].elapsed = 0.0f;
    }
  }
}


void 
lf_ui_core_submit(lf_ui_state_t* ui) {
  ui->root->_needs_rerender = true;
}

void 
lf_ui_core_rerender_widget(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(ui->root->_needs_rerender) return;
  lf_widget_t* rerender = widget;
  if(!rerender->_changed_size) {
    rerender->_needs_rerender = true;
    return;
  }
  while(rerender->parent) {
    lf_widget_t* p = rerender->parent;
    if(p->size_calc){
      vec2s sizebefore = p->container.size; 
      p->size_calc(ui, p);
      p->_changed_size = 
        !(  p->container.size.x == sizebefore.x && 
        p->container.size.y == sizebefore.y);
      
    }
    if(!p->_changed_size) {
      rerender = p;
      break;
    }
    rerender = rerender->parent;
  }
  if(rerender != ui->root) {
    lf_widget_shape(ui, rerender);
  }
  rerender->_needs_rerender = true;
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

  lf_vector_free(&ui->pages);

  lf_widget_remove(ui->root);
  remove_marked_widgets(ui->root);

  lf_win_destroy(ui->win);

  lf_windowing_terminate();

  lf_asset_manager_terminate(ui->asset_manager);

  lf_ez_api_terminate(ui);

#ifdef LF_RUNARA
  rn_terminate((RnState*)ui->render_state);
#endif

  free(ui);
}

void 
lf_ui_core_remove_all_widgets(lf_ui_state_t* ui) {
  lf_widget_remove(ui->root);
  ui->root->_marked_for_removal = false;
}

void 
lf_ui_core_add_page(lf_ui_state_t* ui, lf_page_func_t page_func, const char* identifier) {
  lf_vector_append(
    &ui->pages, 
    ((lf_page_t) {
      .id = lf_djb2_hash((const unsigned char*)identifier),
      .display= page_func 
    }));
}

void 
lf_ui_core_set_page(lf_ui_state_t* ui, const char* identifier) {
  uint64_t id = lf_djb2_hash((const unsigned char*)identifier);
  lf_ui_core_set_page_by_id(ui, id);
  lf_ui_core_submit(ui);
}

void 
lf_ui_core_set_page_by_id(lf_ui_state_t* ui, uint64_t id) {
  if(ui->crnt_page_id == id) return;

  if(ui->crnt_page_id != 0) {
    lf_ui_core_remove_all_widgets(ui);
  }
  lf_page_t* page = NULL;
  for(uint32_t i = 0; i < ui->pages.size; i++) {
    if(ui->pages.items[i].id == id) {
      page = &ui->pages.items[i];
      break;
    }
  }

  if(!page) {
    fprintf(stderr, "leif: cannot set page to %li because a page with this ID does not exist.", id);
    return;
  }

  ui->crnt_page_id = id;
  interrupt_all_animations_recursively(ui->root);

  ui->_root_layout_func(ui);
}

void 
lf_ui_core_remove_page(lf_ui_state_t* ui, const char* identifier) {
  uint64_t id = lf_djb2_hash((const unsigned char*)identifier);
  int32_t page_idx = -1;
  for(uint32_t i = 0; i < ui->pages.size; i++) {
    if(ui->pages.items[i].id == id) {
      page_idx = (int32_t)i;
      break;
    }
  }
  if(page_idx == -1) {
    fprintf(stderr, "leif: cannot remove page '%s' because a page with this ID does not exist.", identifier);
  }

  lf_vector_remove_by_idx(&ui->pages, page_idx);
}

void 
lf_ui_core_display_current_page(lf_ui_state_t* ui) {
  lf_page_t* page = NULL;
  for(uint32_t i = 0; i < ui->pages.size; i++) {
    if(ui->pages.items[i].id == ui->crnt_page_id) {
      page = &ui->pages.items[i];
      break;
    }
  }
  if(!page) {
    fprintf(stderr, "leif: cannot display current page '%li' because a page with this ID does not exist.\n", ui->crnt_page_id);
    return;
  }
  page->display(ui);
}

void 
lf_ui_core_set_root_layout(lf_ui_state_t* ui, lf_page_func_t layout_func) {
  ui->_root_layout_func = layout_func;
}

lf_timer_t* 
lf_ui_core_start_timer(lf_ui_state_t* ui, float duration, lf_timer_finish_func_t finish_cb) {
  lf_vector_append(
    &ui->timers, ((lf_timer_t){
      .duration = duration,
      .elapsed = 0.0f,
      .paused = false,
      .expired = false,
      .looping = false,
      .finish_cb = finish_cb
    }));
  for(uint32_t i = 0; i < ui->timers.size; i++) {
    printf("Timer: %f, %f\n", ui->timers.items[i].duration,
        ui->timers.items[i].elapsed);
  }
  return &ui->timers.items[ui->timers.size - 1];
}

lf_timer_t*
lf_ui_core_start_timer_looped(lf_ui_state_t* ui, float duration, lf_timer_finish_func_t finish_cb) {
  lf_vector_append(
    &ui->timers, ((lf_timer_t){
      .duration = duration,
      .elapsed = 0.0f,
      .paused = false,
      .expired = false,
      .looping = true,
      .finish_cb = finish_cb
    }));
  return &ui->timers.items[ui->timers.size - 1];
}

void 
lf_ui_core_commit_entire_render(lf_ui_state_t* ui) {
  if(!ui) return;
  if(!ui->root->_needs_rerender) return;
  vec2s win_size = lf_win_get_size(ui->win);
  lf_container_t clear_area = LF_SCALE_CONTAINER(
    win_size.x,
    win_size.y);
  ui->root->container = clear_area;
  ui->render_resize_display(ui->render_state, win_size.x, win_size.y);
  render_widget_and_submit(ui, ui->root, clear_area);
  lf_win_swap_buffers(ui->win);
}

