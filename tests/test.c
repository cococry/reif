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
#include <stdlib.h>
#include <stdbool.h>

#include <runara/runara.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct {
  lf_button_t* text_div;
  lf_ui_state_t* ui;
  float widget_alpha;
} state_t;

static state_t s;

void finish_anim(lf_animation_t* anim, void* user_data) {
  (void)anim;
  lf_widget_t* icon = (lf_widget_t*)user_data;
  icon->visible = false;
  lf_ui_core_submit(s.ui);
}

void tick_alpha(lf_animation_t* anim, void* user_data) {
  (void)anim;
  lf_widget_t* widget = (lf_widget_t*)user_data;
  if(widget->type == WidgetTypeText) {
    widget->_initial_props.text_color.a = (uint8_t)s.widget_alpha;
  } else {

    widget->_initial_props.color.a = (uint8_t)s.widget_alpha;
  }
}

void on_button_enter(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(!widget) return;
  (void)ui;
  float start_val = 15.0f; 
  float end_val = 30.0f;
  float anim_time = 0.2f;
  lf_animation_func_t easing = lf_ease_out_quad;
  lf_widget_add_animation(widget, &widget->_initial_props.padding_left, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->_initial_props.padding_right, start_val, end_val, anim_time, easing);
}

void on_button_leave(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(!widget) return;
  (void)ui;
  (void)ui;
  float start_val = 30.0f; 
  float end_val = 15.0f;
  float anim_time = 0.2f;
  lf_animation_func_t easing = lf_ease_out_quad;
  lf_widget_add_animation(widget, &widget->_initial_props.padding_left, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->_initial_props.padding_right, start_val, end_val, anim_time, easing);

}

void fade_widgets_out(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(widget != ui->root) {
    float anim_time = 0.5f;
    lf_animation_func_t easing = lf_ease_out_quad;
    lf_animation_t* anim = lf_widget_add_animation(widget, &s.widget_alpha, 255.0f, 0.0f, anim_time, easing);
    anim->user_data = widget;
    anim->tick_cb = tick_alpha;
  }

  for(uint32_t i = 0; i < widget->num_childs; i++) {
    fade_widgets_out(ui, widget->childs[i]);
  }
}
void on_start_click(lf_ui_state_t* ui, lf_widget_t* widget) {
  (void)widget;
  fade_widgets_out(ui, ui->root);
}

void button(lf_ui_state_t* ui, const char* text, lf_button_func_t on_click) {
  lf_button(ui);
  lf_style_crnt_widget_prop(ui, corner_radius, 18.0f); 
  lf_style_crnt_widget_prop(ui, padding_left, 18.0f); 
  lf_style_crnt_widget_prop(ui, padding_right, 18.0f); 

  ((lf_button_t*)lf_crnt(ui))->on_click = on_click;
  ((lf_button_t*)lf_crnt(ui))->on_enter = on_button_enter;
  ((lf_button_t*)lf_crnt(ui))->on_leave = on_button_leave;

  lf_text_h3(ui, text); 
  lf_style_crnt_widget_prop(ui, text_color, LF_BLACK);
  lf_button_end(ui);
}

void home_page(lf_ui_state_t* ui) {
  lf_text_h1(ui, "Testing the Leif UI Framwork™");
  lf_text_h4(ui, "This program is a capability test of the leif framework.");
  lf_style_crnt_widget_prop(ui, text_color, lf_color_from_hex(0x777777));
  lf_style_crnt_widget_prop(ui, margin_bottom, 20.0f); 

  button(ui, "Start test", on_start_click);
}

void root_layout(lf_ui_state_t* ui) {
  lf_div(ui);

  lf_widget_set_fixed_width_percent(lf_crnt(ui), 100);
  lf_widget_set_fixed_height_percent(lf_crnt(ui), 100);

  lf_style_crnt_widget_prop(ui, color, lf_color_from_hex(0x282828));
  lf_widget_set_alignment(lf_crnt(ui), AlignCenterHorizontal | AlignCenterVertical);
  lf_widget_set_layout(lf_crnt(ui), LayoutVertical);

  lf_ui_core_display_current_page(ui);

  lf_div_end(ui);

}

int main(void) {
  s.widget_alpha = 255.0;

  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t* win = lf_ui_core_create_window(1280, 720, "hello leif");
  s.ui = lf_ui_core_init(win);

  lf_ui_core_set_font(s.ui, "/usr/share/fonts/OTF/Lora-Italic.otf");
  
  lf_ui_core_add_page(s.ui, home_page, "home");

  lf_ui_core_set_root_layout(s.ui, root_layout); 


  while(s.ui->running) {
    lf_ui_core_next_event(s.ui);
  }
 
  lf_ui_core_terminate(s.ui);

  return EXIT_SUCCESS;
}
