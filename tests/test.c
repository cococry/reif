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

typedef void (*widget_func_t)(lf_ui_state_t* ui, lf_widget_t* widget);

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

void do_for_all_childs(lf_ui_state_t* ui, lf_widget_t* widget, widget_func_t func) {
  func(ui, widget);
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    do_for_all_childs(ui, widget->childs[i], func);
  }
}

void fade_widget_out(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(widget != ui->root) {
    float anim_time = 0.5f;
    lf_animation_func_t easing = lf_ease_out_quad;
    lf_animation_t* anim = lf_widget_add_animation(widget, &s.widget_alpha, 255.0f, 0.0f, anim_time, easing);
    anim->user_data = widget;
    anim->tick_cb = tick_alpha;
  }
}

void fade_widget_in(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(widget != ui->root) {
    float anim_time = 0.5f;
    lf_animation_func_t easing = lf_ease_out_quad;
    lf_animation_t* anim = lf_widget_add_animation(widget, &s.widget_alpha, 0.0f, 255.0f, anim_time, easing);
    anim->user_data = widget;
    anim->tick_cb = tick_alpha;
  }
}

void make_widget_transparent(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(widget != ui->root) {
    if(widget->type == WidgetTypeText) {
      widget->props.text_color.a = 0.0f;
    } else {
      widget->props.color.a = 0.0f;
    }
  }
}

void finish_timer(lf_ui_state_t* ui, lf_timer_t* t) {
  (void)t;
  (void)ui;
  lf_ui_core_set_page(ui, "popup_test");
  do_for_all_childs(ui, ui->root, fade_widget_in); 
}
void on_start_click(lf_ui_state_t* ui, lf_widget_t* widget) {
  (void)widget;
  do_for_all_childs(ui, ui->root, fade_widget_out); 
  lf_ui_core_start_timer(ui, 0.7f, finish_timer); 
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

void 
popup_test_page(lf_ui_state_t* ui) {
  lf_text_h1(ui, "Popup Test");
  lf_style_crnt_widget_prop(ui, margin_bottom, 20.0f); 

  lf_div(ui);
  lf_widget_set_fixed_width(lf_crnt(ui), 550.0f);
  lf_widget_set_fixed_height(lf_crnt(ui), 300.0f);
  lf_style_crnt_widget_prop(ui, color, LF_WHITE); 
  lf_style_crnt_widget_prop(ui, corner_radius, 10.0f); 


  {
    lf_div(ui);
    lf_style_crnt_widget_prop(ui, color, LF_WHITE); 
    lf_style_crnt_widget_prop(ui, padding_right, 0.0f); 
    lf_widget_set_layout(lf_crnt(ui), LayoutHorizontal);
    lf_crnt(ui)->justify_type = JustifyEnd;

    lf_button(ui);
    lf_crnt(ui)->props = ui->theme->text_props;
    lf_style_crnt_widget_prop(ui, color, LF_WHITE); 
    lf_widget_submit_props(lf_crnt(ui));

    lf_text_h3(ui, "x");
    lf_style_crnt_widget_prop(ui, text_color, lf_color_from_hex(0x444444)); 

      lf_button_end(ui);
    lf_div_end(ui);
  }
 
  {
    lf_div(ui);
    lf_style_crnt_widget_prop(ui, color, LF_WHITE); 
    lf_widget_set_alignment(lf_crnt(ui), AlignCenterHorizontal);
    lf_text_h1(ui, "Start Today");
    lf_style_crnt_widget_prop(ui, text_color, LF_BLACK); 
    lf_style_crnt_widget_prop(ui, margin_bottom, 25.0f); 
    
    lf_text_p(ui, "Sample text. Click to select the text box. Click again or double click to start editing the text.");
    lf_style_crnt_widget_prop(ui, text_color, lf_color_from_hex(0x444444));
    lf_style_crnt_widget_prop(ui, margin_bottom, 25.0f); 

    lf_button(ui);
    lf_style_crnt_widget_prop(ui, color, lf_color_from_hex(0xd9535a));
    lf_style_crnt_widget_prop(ui, text_color, LF_WHITE);
    lf_style_crnt_widget_prop(ui, corner_radius, 18.0f);
    lf_style_crnt_widget_prop(ui, padding_left, 30.0f);
    lf_style_crnt_widget_prop(ui, padding_right, 30.0f);
    lf_style_crnt_widget_prop(ui, margin_bottom, 25.0f); 

    lf_text_h3(ui, "GET STARTED");

    lf_button_end(ui);

    lf_text_p(ui, "No thanks, I want to learn more about the product");
    lf_style_crnt_widget_prop(ui, text_color, lf_color_from_hex(0x444444)); 


    lf_div_end(ui);
  }
  lf_div_end(ui);

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

  lf_ui_core_set_font(s.ui, "/usr/share/fonts/TTF/DejaVuSansMNerdFont-Regular.ttf");

  lf_style_crnt_widget_prop(s.ui, color, lf_color_from_hex(0x282828));

  const char* icons[] = {
    "",
    "",
    "󰀄"
  };
  const char* texts[] = {
    "Home",
    "Search",
    "My Account"
  };

  lf_div(s.ui); 
  lf_widget_set_layout(lf_crnt(s.ui), LayoutHorizontal);
  lf_widget_set_margin(lf_crnt(s.ui), 0.0f);
  lf_widget_set_padding(lf_crnt(s.ui), 0.0f);
  lf_widget_set_fixed_height_percent(lf_crnt(s.ui), 100.0f);
  lf_widget_set_fixed_width_percent(lf_crnt(s.ui), 100.0f);

  lf_div(s.ui);
  lf_style_crnt_widget_prop(s.ui, color, lf_color_from_hex(0x3c3836));

  lf_widget_set_fixed_width(lf_crnt(s.ui), 200.0f);
  lf_widget_set_fixed_height_percent(lf_crnt(s.ui), 100.0f);

  for(uint32_t i = 0; i < 3; i++) {
    lf_div(s.ui);
      lf_widget_set_layout(lf_crnt(s.ui), LayoutHorizontal);
      lf_widget_set_alignment(lf_crnt(s.ui), AlignCenterVertical);
      lf_text_h4(s.ui, icons[i]);
      lf_style_crnt_widget_prop(s.ui, margin_right, 15.0f);
      lf_text_h4(s.ui, texts[i]);
    lf_div_end(s.ui);
  }

  lf_div_end(s.ui);

  lf_div(s.ui);
  lf_text_h1(s.ui, "Hello, World!\n");
  lf_div_end(s.ui);

  lf_div_end(s.ui);


  while(s.ui->running) {
    lf_ui_core_next_event(s.ui);
  }
 
  lf_ui_core_terminate(s.ui);

  return EXIT_SUCCESS;
}
