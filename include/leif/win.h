#pragma once

#include "event.h"
#include <cglm/types-struct.h>
#ifdef LF_GLFW 
#include "platform/win_glfw.h"
#else
#error "Invalid windowing system specified (valid windowing systems: LF_GLFW)"
#endif

typedef struct lf_ui_state_t lf_ui_state_t;

typedef void (*lf_win_close_func)(
    lf_ui_state_t*,
    void* 
    );
typedef void (*lf_win_refresh_func)(
    lf_ui_state_t*,
    void*,
    uint32_t, uint32_t
    );
typedef void (*lf_win_resize_func)(
    lf_ui_state_t*,
    void*,
    uint32_t, uint32_t
    );

typedef void (*lf_win_mouse_press_func)(
    lf_ui_state_t*,
    void*,
    int32_t
    );
typedef void (*lf_win_mouse_release_func)(
    lf_ui_state_t*,
    void*,
    int32_t
    );


int32_t lf_windowing_init(void);

int32_t lf_windowing_terminate(void);

void lf_windowing_set_ui_state(lf_ui_state_t* state);

lf_event_type_t lf_windowing_get_current_event(void);

void lf_windowing_next_event(void);

void* lf_win_get_display(void);

lf_window_t* lf_win_create(uint32_t width, uint32_t height, const char* title);

void lf_win_set_title(lf_window_t* win, const char* title);

int32_t lf_win_make_gl_context(lf_window_t* win);

void lf_win_swap_buffers(lf_window_t* win);

void lf_win_destroy(lf_window_t* win);

vec2s lf_win_cursor_pos(lf_window_t* win);

void lf_win_set_close_cb(lf_window_t* win, lf_win_close_func close_cb);

void lf_win_set_refresh_cb(lf_window_t* win, lf_win_refresh_func refresh_cb);

void lf_win_set_resize_cb(lf_window_t* win, lf_win_resize_func resize_cb);

void lf_win_set_mouse_press_cb(lf_window_t* win, lf_win_mouse_press_func mouse_press_cb);

void lf_win_set_mouse_release_cb(lf_window_t* win, lf_win_mouse_release_func mouse_release_cb);

vec2s lf_win_get_size(lf_window_t* win);

int32_t lf_win_get_refresh_rate(lf_window_t* win);
