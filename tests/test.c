#include <GLFW/glfw3.h>
#include <leif/color.h>
#include <leif/ez_api.h>
#include <leif/layout.h>
#include <leif/event.h>
#include <leif/leif.h>
#include <leif/widget.h>
#include <leif/widgets/button.h>
#include <leif/widgets/div.h>
#include <leif/win.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <runara/runara.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct {
  lf_text_t* time_text;
  lf_ui_state_t* ui;
  lf_timer_t* tick_timer;
} state_t;

static state_t s;

static float elapsed = 0.0f;


char* format_time(float time_in_seconds) {
    char* formatted_time = (char*)malloc(10 * sizeof(char)); 
    if (!formatted_time) {
        return NULL; 
    }

    if (time_in_seconds < 0) {
        time_in_seconds = 0;
    }
    int total_seconds = (int)time_in_seconds;
    int minutes = total_seconds / 60;
    int seconds = total_seconds % 60;
    int milliseconds = (int)((time_in_seconds - total_seconds) * 100);

    snprintf(formatted_time, 10, "%02d:%02d,%02d", minutes, seconds, milliseconds);

    return formatted_time;
}


void on_timer(lf_ui_state_t* ui, lf_timer_t* timer) {
  (void)timer;
  (void)ui;
  elapsed += timer->elapsed;
  char* time = format_time(elapsed);
  lf_text_set_label(ui, s.time_text, time);
  free(time);
  ui->root_needs_render = true;
  lf_ui_core_commit_entire_render(ui);
}
void on_play(lf_ui_state_t* ui, lf_widget_t* widget) {
  (void)widget;
  if(s.tick_timer) {
    s.tick_timer->paused = !s.tick_timer->paused;
  } else {
    s.tick_timer = lf_ui_core_start_timer_looped(ui, 1 / 60.0f, on_timer);
  }
}

int main(void) {

  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t* win = lf_ui_core_create_window(500, 815, "hello leif");
  s.ui = lf_ui_core_init(win);

  lf_ui_core_set_font(s.ui, "/usr/share/fonts/OTF/SF-Pro-Text-Bold.otf");

  lf_style_crnt_widget_prop(s.ui, color, LF_BLACK);

  lf_div(s.ui);
  lf_widget_set_fixed_width_percent(lf_crnt(s.ui), 100.0f);
  lf_widget_set_fixed_height_percent(lf_crnt(s.ui), 60.0f);


  lf_font_t bigfont = lf_load_font(s.ui, "/usr/share/fonts/OTF/SF-Pro-Text-Bold.otf", 80);
  lf_font_t icons = lf_load_font(s.ui, "/usr/share/fonts/TTF/IosevkaNerdFont-Regular.ttf", 40);

  lf_div(s.ui);
  lf_crnt(s.ui)->justify_type = JustifySpaceBetween;
  lf_widget_set_layout(lf_crnt(s.ui), LayoutHorizontal);
  lf_text_h2(s.ui, "Stopwatch");
  lf_text_custom_font(s.ui, "", icons);
  lf_div_end(s.ui);
  
  lf_div(s.ui);
  lf_widget_set_alignment(lf_crnt(s.ui), AlignCenterHorizontal);
  s.time_text = lf_text_custom_font(s.ui, "00:00:00", bigfont);
  lf_style_crnt_widget_prop(s.ui, margin_top, 80.0f);
  lf_style_crnt_widget_prop(s.ui, margin_bottom, 80.0f);
  lf_div_end(s.ui);


  lf_div(s.ui);
  lf_widget_set_padding(lf_crnt(s.ui), 40.0f);
  lf_widget_set_margin(lf_crnt(s.ui), 0.0f);
  lf_widget_set_layout(lf_crnt(s.ui), LayoutHorizontal);
  lf_widget_set_alignment(lf_crnt(s.ui), AlignCenterHorizontal | AlignCenterVertical);
  
  lf_button(s.ui);

  lf_style_crnt_widget_prop(s.ui, color, lf_color_from_hex(0xFCFCFC));
  lf_style_crnt_widget_prop(s.ui, border_color, lf_color_from_hex(0xFE7A7A));
  lf_style_crnt_widget_prop(s.ui, border_width, 5.0f);
  lf_style_crnt_widget_prop(s.ui, corner_radius, 40.0f);
  lf_style_crnt_widget_prop(s.ui, margin_right, 30.0f);
  lf_text_custom_font(s.ui, "", icons);
  lf_widget_set_padding(lf_crnt(s.ui), 15.0f);
  lf_style_crnt_widget_prop(s.ui, padding_right, 20.0f); 
  lf_style_crnt_widget_prop(s.ui, text_color, LF_BLACK);

  lf_button_end(s.ui);

  lf_button(s.ui);
  lf_style_crnt_widget_prop(s.ui, color, lf_color_from_hex(0xFE7A7A));
  lf_style_crnt_widget_prop(s.ui, padding_left, 180.0f); 
  lf_style_crnt_widget_prop(s.ui, padding_top, 100.0f); 
  lf_style_crnt_widget_prop(s.ui, corner_radius, 20.0f);
  lf_style_crnt_widget_prop(s.ui, margin_right, 30.0f);
  ((lf_button_t*)lf_crnt(s.ui))->on_click = on_play;

  lf_text_custom_font(s.ui, "", icons);
  lf_style_crnt_widget_prop(s.ui, text_color, LF_BLACK);

  lf_button_end(s.ui);

  lf_button(s.ui);

  lf_style_crnt_widget_prop(s.ui, color, lf_color_from_hex(0xFCFCFC));
  lf_style_crnt_widget_prop(s.ui, border_color, lf_color_from_hex(0xFE7A7A));
  lf_style_crnt_widget_prop(s.ui, border_width, 5.0f);
  lf_style_crnt_widget_prop(s.ui, corner_radius, 40.0f);
  lf_text_custom_font(s.ui, "󰑙", icons);
  lf_widget_set_padding(lf_crnt(s.ui), 15.0f);
  lf_style_crnt_widget_prop(s.ui, padding_left, 20.0f); 
  lf_style_crnt_widget_prop(s.ui, padding_right, 20.0f); 
  lf_style_crnt_widget_prop(s.ui, text_color, LF_BLACK);

  lf_button_end(s.ui);

  lf_div_end(s.ui);

  lf_div(s.ui);
  lf_style_crnt_widget_prop(s.ui, color, lf_color_from_hex(0xFDD6D6));
  lf_widget_set_fixed_height_percent(lf_crnt(s.ui), 60.0f);
  lf_widget_set_fixed_width_percent(lf_crnt(s.ui), 100.0f);
  lf_widget_set_padding(lf_crnt(s.ui), 40.0f);
  lf_style_crnt_widget_prop(s.ui, corner_radius, 35.0f);
  lf_crnt(s.ui)->justify_type = JustifySpaceBetween;
  lf_widget_set_layout(lf_crnt(s.ui), LayoutHorizontal);

  lf_text_h3(s.ui, "#01 - Lap");
  lf_style_crnt_widget_prop(s.ui, text_color, LF_BLACK);
  lf_text_h3(s.ui, "- 00:00 +");
  lf_style_crnt_widget_prop(s.ui, text_color, LF_BLACK);

  lf_div_end(s.ui);


  lf_div_end(s.ui);

  while(s.ui->running) {
    lf_ui_core_next_event(s.ui);
  }
 
  lf_ui_core_terminate(s.ui);

  return EXIT_SUCCESS;
}
