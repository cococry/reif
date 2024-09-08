#pragma once

#ifdef LF_X11
#include "platform/win_x11.h"
#else
#error "Invalid windowing system specified (valid windowing systems: LF_X11)"
#endif

#include "event.h"

int32_t lf_windowing_init();

int32_t lf_windowing_terminate();

Display* lf_win_get_display();

lf_window_t* lf_win_create(uint32_t width, uint32_t height);

void lf_win_set_title(lf_window_t* win, const char* title);

int32_t lf_win_make_gl_context(lf_window_t* win);

void lf_win_swap_buffers(lf_window_t* win);

lf_event_t lf_win_next_event(lf_window_t* win);

void lf_win_destroy(lf_window_t* win);

uint32_t lf_win_get_keycode(KeySym keysym);
