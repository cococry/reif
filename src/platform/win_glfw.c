#include <stdint.h>
#ifdef LF_GLFW
#include "../../include/leif/win.h"
#include "../../include/leif/ui_core.h"
#include "../../include/leif/event.h"
#include "../../include/leif/widget.h"
#include <stdlib.h>
#include <stdio.h>

#include <GLFW/glfw3.h>

#define MAX_WINDOWS 16

#define SET_WINDOW_DATA(win, cb_field, cb_func)                      \
  do {                                                               \
    for (uint32_t i = 0; i < n_windows; ++i) {                       \
      if (window_callbacks[i].win == (win)) {                        \
        window_callbacks[i].cb_field = (cb_func);                    \
        break;                                                       \
      }                                                              \
    }                                                                \
  } while (0)


#define MAX_KEYS GLFW_KEY_LAST
#define MAX_MOUSE_BUTTONS GLFW_MOUSE_BUTTON_LAST

typedef struct {
  lf_win_mouse_press_func ev_mouse_press_cb;
  lf_win_mouse_release_func ev_mouse_release_cb;
  lf_win_refresh_func ev_refresh_cb;
  lf_win_resize_func ev_resize_cb;
  lf_win_close_func ev_close_cb;
  lf_win_mouse_move_func ev_move_cb;
  lf_win_mouse_wheel_func ev_mouse_wheel_cb;
  lf_win_key_func ev_key_cb;
  lf_win_char_func ev_char_cb;

  lf_windowing_event_func windowing_event_cb;
  GLFWwindow* win;
  lf_ui_state_t* ui;
} window_callbacks_t;


static window_callbacks_t window_callbacks[MAX_WINDOWS];
static uint32_t n_windows = 0;
static lf_event_type_t current_event;

static int last_mouse_x = 0;
static int last_mouse_y = 0;

static bool wait_events = true;

static GLFWcursor* cursors[LF_CURSOR_COUNT];

static lf_window_t create_window(
  uint32_t width, 
  uint32_t height, 
  const char* title, 
  uint32_t flags, 
  lf_windowing_hint_kv_t* hints, 
  uint32_t nhints);

static void glfw_mouse_button_callback(
  GLFWwindow* window, 
  int32_t button, 
  int32_t action, 
  int32_t mods); 

static void glfw_resize_callback(
  GLFWwindow* window,
  int32_t w, int32_t h);

static void glfw_close_callback(
  GLFWwindow* window);

static void glfw_mouse_move_callback(
  GLFWwindow* window, double xpos, double ypos);

static void glfw_refresh_callback(GLFWwindow* window);

static void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

static void glfw_key_callback(
  GLFWwindow* window, 
  int32_t key, 
  int32_t scancode, 
  int32_t action,
  int32_t mods);

static void glfw_char_callback(
  GLFWwindow* window, 
  uint32_t charcode);

static window_callbacks_t* win_data_from_native(lf_window_t win);

lf_window_t
create_window(
  uint32_t width, 
  uint32_t height, 
  const char* title, 
  uint32_t flags, 
  lf_windowing_hint_kv_t* hints, 
  uint32_t nhints) {
  (void)flags;
  for(uint32_t i = 0; i < nhints; i++) {
    if(hints[i].key == LF_WINDOWING_HINT_TRANSPARENT_FRAMEBUFFER) 
      glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, hints[i].value);
    if(hints[i].key == LF_WINDOWING_HINT_DECORATED) 
      glfwWindowHint(GLFW_DECORATED, hints[i].value);
    if(hints[i].key == LF_WINDOWING_HINT_RESIZABLE) 
      glfwWindowHint(GLFW_RESIZABLE, hints[i].value);
    if(hints[i].key == LF_WINDOWING_HINT_ABOVE) 
      glfwWindowHint(GLFW_FLOATING, hints[i].value);
  }
  GLFWwindow* win = glfwCreateWindow(
    width, height, 
    title,
    NULL, NULL);


  int32_t win_x, win_y;
  glfwGetWindowPos(win, &win_x, &win_y);
  for(uint32_t i = 0; i < nhints; i++) {
    if(hints[i].key == LF_WINDOWING_HINT_POS_X) {
      glfwSetWindowPos(win, hints[i].value, win_y);
    }
    if(hints[i].key == LF_WINDOWING_HINT_POS_Y) {
      glfwSetWindowPos(win, win_x, hints[i].value);
    }
  }
  if(n_windows + 1 <= MAX_WINDOWS) {
    lf_win_register(win);
  }
  else {
    fprintf(stderr, "warning: reached maximum amount of windows to define callbacks for.\n");
  }

  glfwSwapInterval(1);

  return win;
}

void 
cursor_enter_callback(GLFWwindow* window, int entered) {
  lf_event_t ev = {0};
  window_callbacks_t* data = win_data_from_native(window);
  if (!entered && data->ui) {
    ev.x = -1; 
    ev.y = -1; 
    ev.delta_x = 0; 
    ev.delta_y = 0; 
    ev.type = LF_EVENT_MOUSE_MOVE;
    current_event = ev.type; 
    lf_widget_handle_event(data->ui, data->ui->root, &ev);
    for(uint32_t i = 0; i < n_windows; i++) {
    if (window_callbacks[i].ev_move_cb)
      window_callbacks[i].ev_move_cb(
        data->ui, 
        window_callbacks[i].win,
        -1, -1);
      }
      printf("called here.\n");
  }
}

void 
lf_win_register(lf_window_t win) {
    window_callbacks[n_windows].win = win;
    window_callbacks[n_windows].ev_mouse_press_cb = NULL;
    window_callbacks[n_windows].ev_mouse_release_cb = NULL;
    window_callbacks[n_windows].ev_close_cb = NULL;
    window_callbacks[n_windows].ev_refresh_cb = NULL;
    window_callbacks[n_windows].ev_move_cb = NULL;
    window_callbacks[n_windows].ev_mouse_wheel_cb = NULL;
    window_callbacks[n_windows].ev_key_cb = NULL;
    window_callbacks[n_windows].ev_char_cb = NULL;
    window_callbacks[n_windows].ui = NULL;
    ++n_windows;
    glfwSetMouseButtonCallback(win, glfw_mouse_button_callback);
    glfwSetWindowCloseCallback(win, glfw_close_callback);
    glfwSetFramebufferSizeCallback(win, glfw_resize_callback);
    glfwSetWindowRefreshCallback(win, glfw_refresh_callback);
    glfwSetCursorPosCallback(win, glfw_mouse_move_callback);
    glfwSetScrollCallback(win, glfw_scroll_callback);
    glfwSetKeyCallback(win, glfw_key_callback);
    glfwSetCharCallback(win, glfw_char_callback);
  glfwSetCursorEnterCallback(win, cursor_enter_callback);
}

void 
glfw_mouse_button_callback(
  GLFWwindow* window, 
  int32_t button, 
  int32_t action, 
  int32_t mods) {
  window_callbacks_t* data = win_data_from_native(window);
  if(!data) return;
  double xpos, ypos;
  (void)window;
  (void)mods;
  lf_event_t ev = {0};
  
  ev.button = button;
  ev.type = (action != GLFW_RELEASE) ? LF_EVENT_MOUSE_PRESS : LF_EVENT_MOUSE_RELEASE;

  glfwGetCursorPos(window, &xpos, &ypos);
  if(last_mouse_x == 0) last_mouse_x = xpos;
  if(last_mouse_y == 0) last_mouse_y = ypos;
  ev.x = (uint16_t)xpos;
  ev.y = (uint16_t)ypos;
  ev.delta_x = xpos - last_mouse_x;
  ev.delta_y = ypos - last_mouse_y;
  last_mouse_x = xpos;
  last_mouse_y = ypos;
  lf_widget_handle_event(data->ui, data->ui->root, &ev);
  
  
  if(action != GLFW_RELEASE) {
    for(uint32_t i = 0; i < n_windows; i++) {
      if(window_callbacks[i].win == window && window_callbacks[i].ev_mouse_press_cb)
        window_callbacks[i].ev_mouse_press_cb(data->ui, window, button);
    }
    current_event = LF_EVENT_MOUSE_RELEASE;
  } else {
    for(uint32_t i = 0; i < n_windows; i++) {
      if(window_callbacks[i].win == window && window_callbacks[i].ev_mouse_release_cb)
        window_callbacks[i].ev_mouse_release_cb(data->ui, window, button);
    }
    current_event = LF_EVENT_MOUSE_PRESS;
  }

}

void 
glfw_resize_callback(
  GLFWwindow* window,
  int32_t w, int32_t h) {
  window_callbacks_t* data = win_data_from_native(window);
  if(!data) return;
  lf_event_t ev = {0};
 
  ev.type = LF_EVENT_WINDOW_RESIZE; 
  ev.width = w; ev.height = h;
  current_event = LF_EVENT_WINDOW_RESIZE;
  lf_win_make_gl_context(data->win);
  lf_widget_handle_event(data->ui, data->ui->root, &ev);
  
  for(uint32_t i = 0; i < n_windows; i++) {
      if(window_callbacks[i].win == window && window_callbacks[i].ev_resize_cb)
        window_callbacks[i].ev_resize_cb(data->ui, window, w, h);
  }
}

void 
glfw_refresh_callback(
  GLFWwindow* window) {
  window_callbacks_t* data = win_data_from_native(window);
  if(!data) return;
  lf_event_t ev = {0};
  ev.type = LF_EVENT_WINDOW_REFRESH; 
  current_event = LF_EVENT_WINDOW_REFRESH;
  lf_widget_handle_event(data->ui, data->ui->root, &ev);
  for(uint32_t i = 0; i < n_windows; i++) {
      if(window_callbacks[i].win == window && window_callbacks[i].ev_refresh_cb)
        window_callbacks[i].ev_refresh_cb(data->ui, window);
  }
}

void 
glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  window_callbacks_t* data = win_data_from_native(window);
  if(!data) return;
  lf_event_t ev = {0};
  
  ev.type = LF_EVENT_MOUSE_WHEEL;
  ev.scroll_x = (int16_t)xoffset;
  ev.scroll_y = (int16_t)yoffset;

  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);
  ev.x = (int16_t)xpos;
  ev.y = (int16_t)ypos;

  current_event = LF_EVENT_MOUSE_WHEEL;
  lf_widget_handle_event(data->ui, data->ui->root, &ev);

  for(uint32_t i = 0; i < n_windows; i++) {
    if(window_callbacks[i].win == window && window_callbacks[i].ev_mouse_wheel_cb)
      window_callbacks[i].ev_mouse_wheel_cb(data->ui, window, (int16_t)xoffset, (int16_t)yoffset);
  }
}

void 
glfw_key_callback(
  GLFWwindow* window, 
  int32_t key, 
  int32_t scancode, 
  int32_t action,
  int32_t mods) {
  window_callbacks_t* data = win_data_from_native(window);
  if(!data) return;

  lf_event_t ev = {0};
  ev.keycode = key;
  ev.keyscancode = scancode;
  ev.keyaction = (action == GLFW_RELEASE) ? LF_KEY_ACTION_RELEASE : LF_KEY_ACTION_PRESS;
  ev.keymods = mods;
  ev.type = action == GLFW_RELEASE ? LF_EVENT_KEY_RELEASE : LF_EVENT_KEY_PRESS;
  current_event = ev.type; 

  lf_widget_handle_event(data->ui, data->ui->root, &ev);

  for(uint32_t i = 0; i < n_windows; i++) {
    if(window_callbacks[i].win == window && window_callbacks[i].ev_key_cb)
      window_callbacks[i].ev_key_cb(data->ui, window, key, scancode, ev.keyaction, mods); 
  }
}

void 
glfw_char_callback(
  GLFWwindow* window, 
  uint32_t charcode) {
  window_callbacks_t* data = win_data_from_native(window);
  if(!data) return;
  char utf8[5];
  int32_t utf8len = lf_codepoint_to_utf8(charcode, utf8);

  lf_event_t ev = {0};
  memcpy(ev.charutf8, utf8, 5);
  ev.type = LF_EVENT_TYPING_CHAR;
  
  lf_widget_handle_event(data->ui, data->ui->root, &ev);
  
  for(uint32_t i = 0; i < n_windows; i++) {
    if(window_callbacks[i].win == window && window_callbacks[i].ev_char_cb)
      window_callbacks[i].ev_char_cb(data->ui, window, utf8, utf8len); 
  }
}

window_callbacks_t*
win_data_from_native(lf_window_t win) {
  for(uint32_t i = 0; i < n_windows; i++) {
    if(window_callbacks[i].win == win) {
      return &window_callbacks[i];
    }
  }
  return NULL;
}

void 
glfw_close_callback(GLFWwindow* window) {
  window_callbacks_t* data = win_data_from_native(window);
  if(!data) return;
  for(uint32_t i = 0; i < n_windows; i++) {
      if(window_callbacks[i].win == window && window_callbacks[i].ev_close_cb) {
        window_callbacks[i].ev_close_cb(data->ui, window);
    }
    }
  lf_event_t ev = {0};
  ev.type = LF_EVENT_WINDOW_CLOSE; 
  current_event = LF_EVENT_WINDOW_CLOSE;
  lf_widget_handle_event(data->ui, data->ui->root, &ev);
}

void 
glfw_mouse_move_callback(
  GLFWwindow* window, double xpos, double ypos) {
  window_callbacks_t* data = win_data_from_native(window);
  if(!data) return;
  for(uint32_t i = 0; i < n_windows; i++) {
      if(window_callbacks[i].win == window && window_callbacks[i].ev_move_cb)
        window_callbacks[i].ev_move_cb(data->ui, window, (uint16_t)xpos, (uint16_t)ypos);
  }

  lf_event_t ev = {0}; 
  
  if(last_mouse_x == 0) last_mouse_x = xpos;
  if(last_mouse_y == 0) last_mouse_y = ypos;
  ev.x = (uint16_t)xpos;
  ev.y = (uint16_t)ypos;
  ev.delta_x = xpos - last_mouse_x;
  ev.delta_y = ypos - last_mouse_y;
  last_mouse_x = xpos;
  last_mouse_y = ypos;

  ev.type = LF_EVENT_MOUSE_MOVE;
  current_event = LF_EVENT_MOUSE_MOVE; 
  lf_widget_handle_event(data->ui, data->ui->root, &ev);
}

int32_t 
lf_windowing_init(void) {
  if(!glfwInit()) {
    fprintf(stderr, "reif: cannot initialize GLFW windowing system.\n");
    return 1;
  }
  for(uint32_t i = 0; i < LF_CURSOR_COUNT; i++) {
    cursors[i] = NULL;
  }
  return 0;
}

int32_t 
lf_windowing_terminate(void) {
  for(uint32_t i = 0; i < LF_CURSOR_COUNT; i++) {
    if(cursors[i] != NULL) {
      glfwDestroyCursor(cursors[i]);
    }
  }
  glfwTerminate();
  return 0;
}

void
lf_windowing_update(void) {
  current_event = LF_EVENT_NONE;
}

void 
lf_win_set_ui_state(lf_window_t win, lf_ui_state_t* state) {
  SET_WINDOW_DATA(win, ui, state);
}

lf_event_type_t 
lf_windowing_get_current_event(void) {
  return current_event;
}

void 
lf_windowing_next_event(void) {
  if(wait_events) {
    glfwWaitEvents();
  } else {
    glfwPollEvents();
  }
  for(uint32_t i = 0; i < n_windows; i++) {
    if(window_callbacks[i].windowing_event_cb) {
      window_callbacks[i].windowing_event_cb(&current_event, window_callbacks[i].ui);
    }
  }
}

void
lf_windowing_wake_up_event_loop(void) {
  // not implement in GLFW windowing as glfw handles this internally 
}
void 
lf_windowing_set_wait_events(bool wait) {
  wait_events = wait;
}

lf_window_t 
lf_win_create(uint32_t width, uint32_t height, const char* title) {
  return create_window(width, height, title, 0, NULL, 0);
}

lf_window_t 
lf_win_create_ex(uint32_t width, uint32_t height, const char* title, uint32_t flags, 
                 lf_windowing_hint_kv_t* hints, uint32_t nhints) {
  (void)flags;
  return create_window(width, height, title, flags, hints, nhints);
}

void 
lf_win_set_title(lf_window_t win, const char* title) {
  glfwSetWindowTitle(win, title); 
}

int32_t 
lf_win_make_gl_context(lf_window_t win) {
  glfwMakeContextCurrent(win);
  return 0;
}

void 
lf_win_swap_buffers(lf_window_t win) {
  glfwSwapBuffers(win);
}

void 
lf_win_destroy(lf_window_t win) {
  glfwDestroyWindow(win);
  glfwPostEmptyEvent();
}

vec2s 
lf_win_cursor_pos(lf_window_t win) {
  double x, y;
  glfwGetCursorPos(win, &x, &y);
  vec2s cursor = (vec2s){
    .x = (float)x,
    .y = (float)y};
  return cursor;
}

void 
lf_win_set_close_cb(lf_window_t win, lf_win_close_func close_cb) {
  SET_WINDOW_DATA(win, ev_close_cb, close_cb);
}

void 
lf_win_set_refresh_cb(lf_window_t win, lf_win_refresh_func refresh_cb) {
  SET_WINDOW_DATA(win, ev_refresh_cb, refresh_cb);
}

void 
lf_win_set_resize_cb(lf_window_t win, lf_win_resize_func resize_cb) {
  SET_WINDOW_DATA(win, ev_resize_cb, resize_cb);
}

void 
lf_win_set_mouse_press_cb(lf_window_t win, lf_win_mouse_press_func mouse_press_cb) {
  SET_WINDOW_DATA(win, ev_mouse_press_cb, mouse_press_cb);
}

void 
lf_win_set_mouse_release_cb(lf_window_t win, lf_win_mouse_release_func mouse_release_cb) {
  SET_WINDOW_DATA(win, ev_mouse_release_cb, mouse_release_cb);
}

void 
lf_win_set_mouse_move_cb(lf_window_t win, lf_win_mouse_move_func mouse_move_cb) {
  SET_WINDOW_DATA(win, ev_move_cb, mouse_move_cb);
}

void 
lf_win_set_key_cb(lf_window_t win, lf_win_key_func key_cb) {
  SET_WINDOW_DATA(win, ev_key_cb, key_cb);
}

void 
lf_win_set_typing_char_cb(lf_window_t win, lf_win_char_func char_cb) {
  SET_WINDOW_DATA(win, ev_char_cb, char_cb);
}

void 
lf_win_set_event_cb(lf_window_t win, lf_windowing_event_func windowing_cb) {
  SET_WINDOW_DATA(win, windowing_event_cb, windowing_cb);
}

vec2s 
lf_win_get_size(lf_window_t win) {
  int32_t width, height;
  glfwGetWindowSize(win, &width, &height);
  return (vec2s){
    .x = (float)width,
    .y = (float)height,
  };
}

int32_t 
lf_win_get_refresh_rate(lf_window_t win) {
    int window_x, window_y;
    glfwGetWindowPos(win, &window_x, &window_y);

    int monitor_count;
    GLFWmonitor** monitors = glfwGetMonitors(&monitor_count);
    for (int i = 0; i < monitor_count; i++) {
        int monitor_x, monitor_y;
        const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);

        glfwGetMonitorPos(monitors[i], &monitor_x, &monitor_y);

        if (window_x >= monitor_x && window_x < monitor_x + mode->width &&
            window_y >= monitor_y && window_y < monitor_y + mode->height) {
            return mode->refreshRate; 
        }
    }
    return -1;
}


void 
lf_win_hide(lf_window_t win) {
  glfwHideWindow(win);
} 

void 
lf_win_show(lf_window_t win) {
  glfwShowWindow(win);
}

void 
lf_win_set_width(lf_window_t win, float width) {
  glfwSetWindowSize(win, width, lf_win_get_size(win).y); 
}

void 
lf_win_set_height(lf_window_t win, float height) {
  glfwSetWindowSize(win, lf_win_get_size(win).x, height); 
}

void 
lf_win_set_cursor(lf_window_t win, lf_cursor_type_t cursor_type) {
  uint32_t idx = 0;
  switch(cursor_type) {
    case LF_CURSOR_ARROW: {idx = 0; break;}
    case LF_CURSOR_IBEAM: {idx = 1; break;}
    case LF_CURSOR_CROSSHAIR: {idx = 2; break;}
    case LF_CURSOR_HAND: {idx = 3; break;}
    case LF_CURSOR_HRESIZE: {idx = 4; break;}
    case LF_CURSOR_VRESIZE: {idx = 5; break;}
    default: {
        fprintf(stderr, "reif: invalid cursor type specified.\n"); 
        return;
      }
  }
  GLFWcursor* cursor = cursors[idx];
  if(!cursor) {
    cursor = glfwCreateStandardCursor(cursor_type);
  }
  glfwSetCursor(win, cursor);
}


void 
lf_win_reset_cursor(lf_window_t win) {
  glfwSetCursor(win, NULL);
}
#endif
