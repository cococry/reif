#include <GLFW/glfw3.h>
#include <leif/asset_manager.h>
#include <leif/color.h>
#include <leif/ez_api.h>
#include <leif/layout.h>
#include <leif/event.h>
#include <leif/leif.h>
#include <leif/ui_core.h>
#include <leif/util.h>
#include <leif/widget.h>
#include <leif/widgets/button.h>
#include <leif/widgets/div.h>
#include <leif/widgets/text.h>
#include <leif/win.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include <runara/runara.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct {
  lf_ui_state_t* ui;
  lf_widget_t* div;
} state_t;

static state_t s;


static void maincomp(void);

/*
int counter = 0;
void on_click(lf_ui_state_t* ui, lf_widget_t* widget) {
  counter++;
  lf_component_rerender(ui, maincomp);
}
void on_click_dec(lf_ui_state_t* ui, lf_widget_t* widget) {
  counter--;
  lf_component_rerender(ui, maincomp);
}

void maincomp(void) {
  lf_div(s.ui);
  lf_widget_set_font_family(s.ui, lf_crnt(s.ui), "CaskaydiaCove Nerd Font");
  lf_crnt(s.ui)->sizing_type = SizingFitToContent;
  lf_widget_set_alignment(lf_crnt(s.ui), AlignCenterHorizontal);

  char buf[64];
  sprintf(buf, "Counter: %i", counter);
  lf_text_h1(s.ui, buf);

  lf_div(s.ui);
  
  lf_widget_set_font_style(s.ui, lf_crnt(s.ui), LF_FONT_STYLE_BOLD);
  lf_crnt(s.ui)->sizing_type = SizingFitToContent;
  lf_widget_set_layout(lf_crnt(s.ui), LayoutHorizontal);
  lf_button(s.ui);
  if(counter % 2 == 0) {
    lf_widget_set_padding(lf_crnt(s.ui), 20);
  } else {
    lf_widget_set_padding(lf_crnt(s.ui), s.ui->theme->button_props.padding_left);
  }
  lf_style_crnt_widget_prop(s.ui, corner_radius, 15);
  ((lf_button_t*)lf_crnt(s.ui))->on_click = on_click;

  lf_text_h3(s.ui, "Increase");
  lf_button_end(s.ui);

  lf_button(s.ui);
  lf_style_crnt_widget_prop(s.ui, corner_radius, 15);
  ((lf_button_t*)lf_crnt(s.ui))->on_click = on_click_dec;
  lf_text_h3(s.ui, "Decrease");
  lf_button_end(s.ui);


  lf_div_end(s.ui);

  lf_div_end(s.ui);
}
*/


static void comp(void);
static bool a = false;

void on_hover(lf_ui_state_t* ui, lf_widget_t* widget) {
  lf_widget_add_animation(widget, &widget->props.padding_left, ui->theme->button_props.padding_left, 
                          ui->theme->button_props.padding_left + 10, 0.2, lf_ease_in_quad);
  lf_widget_add_animation(widget, &widget->props.padding_right, ui->theme->button_props.padding_right, 
                          ui->theme->button_props.padding_right + 10, 0.2, lf_ease_in_quad);
  lf_widget_add_animation(widget, &widget->props.padding_top, ui->theme->button_props.padding_top, 
                          ui->theme->button_props.padding_top + 10, 0.2, lf_ease_in_quad);
  lf_widget_add_animation(widget, &widget->props.padding_bottom, ui->theme->button_props.padding_top, 
                          ui->theme->button_props.padding_top + 10, 0.2, lf_ease_in_quad);
}

void on_leave(lf_ui_state_t* ui, lf_widget_t* widget) {
  lf_widget_add_animation(widget, &widget->props.padding_left, 
                          ui->theme->button_props.padding_left + 10, 
                          ui->theme->button_props.padding_left, 0.2, lf_ease_in_quad);
  lf_widget_add_animation(widget, &widget->props.padding_right, ui->theme->button_props.padding_right, 
                          ui->theme->button_props.padding_right + 10, 0.2, lf_ease_in_quad);
  
  lf_widget_add_animation(widget, &widget->props.padding_top, ui->theme->button_props.padding_top + 10, 
                          ui->theme->button_props.padding_top , 0.2, lf_ease_in_quad);
  lf_widget_add_animation(widget, &widget->props.padding_bottom, ui->theme->button_props.padding_top + 10, 
                          ui->theme->button_props.padding_top , 0.2, lf_ease_in_quad);
}

void on_click(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(a) {
    lf_widget_set_padding(widget, ui->theme->button_props.padding_left);
  } else {
    lf_widget_set_padding(widget, 20); 
  }
  a = !a;
  lf_component_rerender(ui, comp);
}

static int barsize = 30;
static int numdesktops = 9;
static int crntdesktop = 3;

void comp(void) {

  lf_div(s.ui);
  lf_widget_set_fixed_height(lf_crnt(s.ui), barsize);
  lf_style_crnt_widget_prop(s.ui, padding_top, 0); 
  lf_style_crnt_widget_prop(s.ui, padding_bottom, 0); 
  lf_crnt(s.ui)->sizing_type = SizingFitToContent;
  lf_widget_set_layout(lf_crnt(s.ui), LayoutHorizontal);
  lf_widget_set_alignment(lf_crnt(s.ui), 
      AlignCenterVertical);

  lf_widget_set_margin(lf_crnt(s.ui), 0);

  for(uint32_t i = 0; i < numdesktops; i++) {
    lf_div(s.ui);
    lf_widget_set_padding(lf_crnt(s.ui), 0);
    lf_widget_set_margin(lf_crnt(s.ui), 0);
    if(i != numdesktops - 1)
      lf_style_crnt_widget_prop(s.ui, margin_right, 10);
    lf_widget_set_fixed_width(lf_crnt(s.ui), i != crntdesktop ? 10 : 20); 
    lf_widget_set_fixed_height(lf_crnt(s.ui), 10);
    lf_style_crnt_widget_prop(s.ui, color, 
        (i != crntdesktop) ? 
        lf_color_from_hex(0xbdbdbd) :
        lf_color_from_hex(0xd9d9d9));
    lf_style_crnt_widget_prop(s.ui, corner_radius, 10 / 2.0);
    lf_div_end(s.ui);
  }

  lf_div_end(s.ui);
}

static void desktop_up(lf_ui_state_t* ui, lf_widget_t* widget) {
  crntdesktop++;
  if(crntdesktop >= numdesktops)
    crntdesktop = 0;
  lf_component_rerender(s.ui, comp);
}
int main(void) {

  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t win = lf_ui_core_create_window(1280, 720, "hello leif");
  s.ui = lf_ui_core_init(win);

  lf_style_crnt_widget_prop(s.ui, text_color, LF_BLACK);
  lf_style_crnt_widget_prop(s.ui, color, LF_WHITE);
  lf_widget_set_fixed_height_percent(lf_crnt(s.ui), 100.0f);
  lf_widget_set_alignment(lf_crnt(s.ui), AlignCenterHorizontal | AlignCenterVertical); 

  lf_text_h1(s.ui, "Bar Example");
  lf_style_crnt_widget_prop(s.ui, margin_bottom, 25.0f);

  lf_button(s.ui);
  lf_style_crnt_widget_prop(s.ui, margin_bottom, 25.0f);
  lf_style_crnt_widget_prop(s.ui, corner_radius, 20);
  ((lf_button_t*)lf_crnt(s.ui))->on_click = desktop_up;
  ((lf_button_t*)lf_crnt(s.ui))->on_enter = on_hover;
  ((lf_button_t*)lf_crnt(s.ui))->on_leave = on_leave;
  lf_style_crnt_widget_prop(s.ui, padding_bottom, 5); 
  lf_style_crnt_widget_prop(s.ui, padding_top, 5); 
  lf_text_h4(s.ui, "Desktop Up");
  lf_button_end(s.ui);


  lf_component(s.ui, comp); 

  while(s.ui->running) {
    lf_ui_core_next_event(s.ui);
  }

  lf_ui_core_terminate(s.ui);
	
  return EXIT_SUCCESS;
}
