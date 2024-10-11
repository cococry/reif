#pragma once

#include <GLFW/glfw3.h>

typedef struct lf_ui_state_t lf_ui_state_t;

typedef void (*lf_win_close_func)(
    lf_ui_state_t* ui,
    void* 
    );
typedef void (*lf_win_refresh_func)(
    lf_ui_state_t* ui,
    void*
    );

typedef void (*lf_win_mouse_press_func)(
    lf_ui_state_t* ui,
    void*,
    int32_t
    );
typedef void (*lf_win_mouse_release_func)(
    lf_ui_state_t* ui,
    void*,
    int32_t
    );

typedef struct GLFWwindow lf_window_t;
