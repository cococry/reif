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
  int32_t counter;
  lf_button_t* text_div;
  float icon_alpha;
  lf_ui_state_t* ui;
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
  lf_widget_t* icon = (lf_widget_t*)user_data;
  icon->_initial_props.text_color.a = (uint8_t)s.icon_alpha;
}

void on_button_enter(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(!widget) return;
  (void)ui;
  float start_val = 10.0f; 
  float end_val = 25.0f;
  float anim_time = 0.2f;
  lf_animation_func_t easing = lf_ease_out_quad;
  lf_widget_add_animation(widget, &widget->_initial_props.padding_left, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->_initial_props.padding_right, start_val, end_val, anim_time, easing);

  lf_widget_t* icon = widget->childs[1];
  icon->visible = true;
  lf_widget_add_animation(icon, &icon->_initial_props.margin_left, -10.0f, 10.0f, anim_time, easing);
  lf_animation_t* anim = lf_widget_add_animation(icon, &s.icon_alpha, 0.0f, 255.0f, anim_time, easing);
  anim->user_data = icon;
  anim->tick_cb = tick_alpha;
}

void on_button_leave(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(!widget) return;
  (void)ui;
  (void)ui;
  float start_val = 25.0f; 
  float end_val = 10.0f;
  float anim_time = 0.2f;
  lf_animation_func_t easing = lf_ease_out_quad;
  lf_widget_add_animation(widget, &widget->_initial_props.padding_left, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->_initial_props.padding_right, start_val, end_val, anim_time, easing);

  lf_widget_t* icon = widget->childs[1];
  {
    lf_animation_t* anim = lf_widget_add_animation(icon, &icon->_initial_props.margin_left, 10.0f, -10.0f, anim_time, easing);
    anim->user_data = icon; 
    anim->finish_cb = finish_anim;
  }
  {
    lf_animation_t* anim = lf_widget_add_animation(icon, &s.icon_alpha, 255.0f, 0.0f, anim_time, easing);
    anim->user_data = icon;
    anim->tick_cb = tick_alpha;
  }
}

void on_button_click(lf_ui_state_t* ui, lf_widget_t* widget) {
  (void)widget;
  if(widget) {
    lf_widget_remove(widget);
    s.text_div = NULL;
  }
  lf_ui_core_submit(ui);
}

void listener(lf_ui_state_t* ui, lf_widget_t* widget, lf_event_t ev) {
  (void)ui;
  lf_widget_set_fixed_width(widget, ev.width);
}
int main(void) {
  s.counter = 0;
  s.icon_alpha = 0.0f;

  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t* win = lf_ui_core_create_window(1280, 720, "hello leif");
  s.ui = lf_ui_core_init(win);

  lf_ui_core_set_font(s.ui, "/usr/share/fonts/TTF/IosevkaNerdFont-BoldItalic.ttf");
  
  lf_div(s.ui);

  lf_widget_set_fixed_width_percent(lf_crnt(s.ui), 100);
  lf_widget_set_fixed_height_percent(lf_crnt(s.ui), 100);

  lf_style_crnt_widget_prop(s.ui, color, lf_color_from_hex(0x282828));
  lf_widget_set_alignment(lf_crnt(s.ui), AlignCenterHorizontal | AlignCenterVertical);
  lf_widget_set_layout(lf_crnt(s.ui), LayoutVertical);


  lf_text_h1(s.ui, "Email button test");
  lf_style_crnt_widget_prop(s.ui, margin_left, 0); 
  lf_style_crnt_widget_prop(s.ui, margin_right, 0); 

  lf_text_h4(s.ui, "Try hovering the button.");
  
  lf_style_crnt_widget_prop(s.ui, margin_left, 0); 
  lf_style_crnt_widget_prop(s.ui, margin_right, 0); 
  lf_style_crnt_widget_prop(s.ui, text_color, lf_color_from_hex(0x777777));
  lf_style_crnt_widget_prop(s.ui, margin_bottom, 20.0f);

  lf_button_t* btn = lf_button(s.ui);
  ((lf_button_t*)lf_crnt(s.ui))->on_click = on_button_click;
  ((lf_button_t*)lf_crnt(s.ui))->on_enter = on_button_enter;
  ((lf_button_t*)lf_crnt(s.ui))->on_leave = on_button_leave;
  lf_style_crnt_widget_prop(s.ui, color, lf_color_from_hex(0x0B57D0));
  lf_text_h3(s.ui, "Send"); 
  lf_text_p(s.ui, ""); 
  lf_crnt(s.ui)->visible = false;
  lf_button_end(s.ui);

  lf_style_widget_prop(s.ui, &btn->base, corner_radius, 25);

  lf_div_end(s.ui);



  while(s.ui->running) {
    lf_ui_core_next_event(s.ui);
  }
 
  lf_ui_core_terminate(s.ui);

  return EXIT_SUCCESS;
}
