#pragma once

#include "event.h"
#include <cglm/types-struct.h>
#ifdef LF_GLFW 
#include "platform/win_glfw.h"
#include "platform/input_glfw.h"
#elif defined(LF_X11)
#include "platform/win_x11.h"
#include "platform/input_x11.h"
#else
#error "Invalid windowing system specified (valid windowing systems: LF_GLFW, LF_X11)"
#endif

typedef struct lf_ui_state_t lf_ui_state_t;

typedef enum {
  LF_WINDOWING_FLAG_NONE = 0,
#ifdef LF_X11
  LF_WINDOWING_FLAG_X11_OVERRIDE_REDIRECT = 1 << 0,
#endif 
} lf_windowing_flags_t;

typedef enum {
  LF_WINDOWING_HINT_POS_X = 0,
  LF_WINDOWING_HINT_POS_Y,
  LF_WINDOWING_HINT_TRANSPARENT_FRAMEBUFFER,
  LF_WINDOWING_HINT_DECORATED,
  LF_WINDOWING_HINT_RESIZABLE,
  LF_WINDOWING_HINT_VISIBLE,
  LF_WINDOWING_HINT_ABOVE,
  LF_WINDOWING_HINT_BELOW,
  LF_WINDOWING_HINT_BORDER_WIDTH,
  LF_WINDOWING_HINT_BORDER_COLOR
} lf_window_hint_t;

typedef struct {
  lf_window_hint_t key;
  uint32_t value;
} lf_windowing_hint_kv_t;

typedef void (*lf_win_close_func)(
    lf_ui_state_t*,
    lf_window_t 
    );
typedef void (*lf_win_refresh_func)(
    lf_ui_state_t*,
    lf_window_t
    );
typedef void (*lf_win_resize_func)(
    lf_ui_state_t*,
    lf_window_t,
    uint32_t, uint32_t
    );

typedef void (*lf_win_mouse_press_func)(
    lf_ui_state_t*,
    lf_window_t,
    int32_t
    );
typedef void (*lf_win_mouse_release_func)(
    lf_ui_state_t*,
    lf_window_t,
    int32_t
    );

typedef void (*lf_win_mouse_move_func)(
    lf_ui_state_t*,
    lf_window_t,
    uint16_t x, uint16_t y
    );

typedef void (*lf_windowing_event_func)(
    void* ev,
    lf_ui_state_t* ui
    );

typedef void (*lf_win_mouse_wheel_func)(
    lf_ui_state_t*,
    lf_window_t,
    int16_t scroll_x, int16_t scroll_y
    );

typedef void (*lf_win_key_func)(
    lf_ui_state_t*,
    lf_window_t,
    int32_t key, 
    int32_t scancode,
    int32_t action,
    int32_t mods
    );

typedef void (*lf_win_char_func)(
    lf_ui_state_t*,
    lf_window_t,
    char* utf8,
    uint32_t utf8charlen
    );

int32_t lf_windowing_init(void);

int32_t lf_windowing_terminate(void);

void lf_windowing_update(void);

void lf_windowing_set_wait_events(bool wait);

void lf_win_set_ui_state(lf_window_t win, lf_ui_state_t* state);

lf_event_type_t lf_windowing_get_current_event(void);

void lf_windowing_wake_up_event_loop(void);

void lf_windowing_next_event(void);

#ifdef LF_X11
void* lf_win_get_x11_display(void);
#endif 

lf_window_t lf_win_create(uint32_t width, uint32_t height, const char* title);

lf_window_t lf_win_create_ex(
    uint32_t width, 
    uint32_t height, 
    const char* title, 
    uint32_t flags, 
    lf_windowing_hint_kv_t* hints,
    uint32_t nhints);

void lf_win_set_title(lf_window_t win, const char* title);

int32_t lf_win_make_gl_context(lf_window_t win);

void lf_win_swap_buffers(lf_window_t win);

void lf_win_destroy(lf_window_t win);

vec2s lf_win_cursor_pos(lf_window_t win);

void lf_win_set_close_cb(lf_window_t win, lf_win_close_func close_cb);

void lf_win_set_refresh_cb(lf_window_t win, lf_win_refresh_func refresh_cb);

void lf_win_set_resize_cb(lf_window_t win, lf_win_resize_func resize_cb);

void lf_win_set_mouse_press_cb(lf_window_t win, lf_win_mouse_press_func mouse_press_cb);

void lf_win_set_mouse_release_cb(lf_window_t win, lf_win_mouse_release_func mouse_release_cb);

void lf_win_set_mouse_move_cb(lf_window_t win, lf_win_mouse_move_func mouse_move_cb);

void lf_win_set_key_cb(lf_window_t win, lf_win_key_func key_cb);

void lf_win_set_typing_char_cb(lf_window_t win, lf_win_char_func char_cb);

void lf_win_set_event_cb(lf_window_t win, lf_windowing_event_func windowing_cb);

vec2s lf_win_get_size(lf_window_t win);

int32_t lf_win_get_refresh_rate(lf_window_t win);


void lf_win_hide(lf_window_t win);

void lf_win_show(lf_window_t win);

void lf_win_set_width(lf_window_t win, float width);

void lf_win_set_height(lf_window_t win, float height);

#ifdef LF_GLFW
void lf_win_register(lf_window_t win);
#elif defined(LF_X11)
#include <GL/glx.h>
void lf_win_register(lf_window_t win, GLXContext glcontext, uint32_t flags);
#endif 

void lf_win_set_cursor(lf_window_t win, lf_cursor_type_t cursor_type);

void lf_win_reset_cursor(lf_window_t win);

