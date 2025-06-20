#include "../../include/leif/widgets/input.h"
#include <runara/runara.h>

static void _input_handle_event(
  lf_ui_state_t* ui, 
  lf_widget_t* widget, 
  lf_event_t* event);

static void _input_render(
  lf_ui_state_t* ui,
  lf_widget_t* widget);

static void _input_shape(
  lf_ui_state_t* ui, 
  lf_widget_t* widget);

static void _input_size_calc(
  lf_ui_state_t* ui, 
  lf_widget_t* widget);

static void _do_cursor_blink(
  lf_ui_state_t* ui, 
  lf_timer_t* timer
);

int32_t
utf8insert(char *str, size_t str_capacity, size_t insert_index,
           const char *charutf8, uint32_t charutf8len) {
  size_t str_len = strlen(str);
  if (insert_index > str_len || charutf8len > 4)
    return -1;
  if (str_len + charutf8len >= str_capacity)
    return -1;
  memmove(str + insert_index + charutf8len,
          str + insert_index,
          str_len - insert_index + 1); 
  memcpy(str + insert_index, charutf8, charutf8len);
  return 0;
}

void 
_input_handle_event(
  lf_ui_state_t* ui, 
  lf_widget_t* widget, 
  lf_event_t* event) {
  if(!lf_container_intersets_container(
    widget->container, ui->root->container)) {
    return;
  }

  vec2s mouse = (vec2s){
    .x = (float)event->x, 
    .y = (float)event->y}; 

  lf_container_t container = (lf_container_t){
    .pos = widget->container.pos, 
    .size = LF_WIDGET_SIZE_V2(widget)
  };

  lf_input_t* input = (lf_input_t*)widget;
  bool on_input = lf_point_intersets_container(mouse, container);

  if(input->_focused && event->type == LF_EVENT_TYPING_CHAR) {
    utf8insert(
      input->buf, 512, strlen(input->buf), event->charutf8,
      event->charutf8len);
    lf_text_set_label(ui, input->_text_widget, input->buf);
    ui->needs_render = true;
    input->_show_cursor = true; 
    input->_blink_held = true;
    printf("Got key event: %s\n", event->charutf8);
    input->_cursor_idx++;
  }

  if(on_input && event->type == LF_EVENT_MOUSE_PRESS && !input->_focused) {
    input->_focused = true;
    ui->needs_render = true;
  }
  else if(!on_input && event->type == LF_EVENT_MOUSE_PRESS && input->_focused) {
    input->_focused = false;
    ui->needs_render = true;
  }

  if(!on_input && event->type == LF_EVENT_MOUSE_MOVE && input->_hovered) {
    input->_hovered = false;
    lf_win_set_cursor(ui->win, LF_CURSOR_ARROW); 
  }
  if(on_input && event->type == LF_EVENT_MOUSE_MOVE &&
    !input->_hovered && ui->active_widget_id == 0) {
    input->_hovered = true;

    lf_win_set_cursor(ui->win, LF_CURSOR_IBEAM);
  }

}

void strtill(const char* src, char* dest, uint32_t len, uint32_t index) {
  for(uint32_t i = 0; i <= index; i++) {
    dest[i] = src[i];
  }
  dest[index + 1] = '\0';
}

void
_input_render(
  lf_ui_state_t* ui,
  lf_widget_t* widget) { 
  if(!widget) return;
  if(widget->type != LF_WIDGET_TYPE_INPUT) return;

  lf_input_t* input = (lf_input_t*)widget;
  ui->render_rect(
    ui->render_state, 
    widget->container.pos,
    (vec2s){
      .x = widget->container.size.x + widget->props.padding_left + widget->props.padding_right,
      .y = widget->container.size.y + widget->props.padding_top + widget->props.padding_bottom
    },
    widget->_rendered_props.color, widget->props.border_color,
    widget->props.border_width, widget->props.corner_radius);

  if(input->_focused && input->_show_cursor) {
    if(!input->_blinktimer) {
      input->_blinktimer = lf_ui_core_start_timer_looped(ui, 1.0f, _do_cursor_blink);
      input->_blinktimer->user_data = input;
    }
    float overflow = (widget->container.pos.x + widget->props.padding_left + input->_text_widget->_text_dimension.width) - 
      (widget->container.pos.x + widget->container.size.x + widget->props.padding_left + widget->props.padding_right);
    if (overflow > 0) {
      input->_text_widget->base.container.pos.x = 
      widget->container.pos.x + widget->props.padding_left - overflow - 2;
    } else {
      input->_text_widget->base.container.pos.x = 
      widget->container.pos.x + widget->props.padding_left; 
    }
    FT_Face face = input->_text_widget->font.font->face;
    int font_height_px = 
      (face->size->metrics.ascender - face->size->metrics.descender) / 64;
    char buf[strlen(input->_text_widget->label)];
    strtill(input->_text_widget->label, buf, strlen(input->_text_widget->label),
            input->_cursor_idx
            );
    printf("Text behind cursor: %s\n", buf);

    lf_widget_t* text = &input->_text_widget->base;
    float wrap = text->parent->container.pos.x +
      text->parent->scroll_offset.x + text->parent->container.size.x +
      text->parent->props.padding_left / 2.0f; 
    if(text->parent->sizing_type == LF_SIZING_FIT_CONTENT || text->_positioned_absolute_x) {
      wrap = -1.0f;
    }
    float textwidth = ui->render_get_paragraph_dimension(
      ui->render_state, buf, input->_text_widget->base.container.pos, 
      input->_text_widget->font.font, (lf_paragraph_props_t){.align = ParagraphAlignmentLeft,
      .wrap = wrap}).width; 
    float cursoroffset = textwidth; 
    if(input->_text_widget->_text_dimension.width > widget->container.size.x) {
      cursoroffset = widget->container.size.x - widget->props.padding_right - 2;
    }
    ui->render_rect(
      ui->render_state,
      (vec2s){
        .x  = widget->container.pos.x + widget->props.padding_left + 
        cursoroffset,
        .y =  widget->container.pos.y + widget->props.padding_top
      },
      (vec2s){
        .x = 1, 
        .y =  
        font_height_px
      }, 
      input->_text_widget->base.props.text_color,
      LF_NO_COLOR, 0.0f, 0.0f);
  }
  

}

void 
_input_shape(lf_ui_state_t* ui, lf_widget_t* widget) {
  (void)ui;
  if(!widget) return;
  if(widget->type != LF_WIDGET_TYPE_INPUT) return;
  lf_widget_apply_layout(ui, widget);
}

void 
_input_size_calc(lf_ui_state_t* ui, lf_widget_t* widget) {
  (void)ui;
  if(!widget) return;
  if(widget->type != LF_WIDGET_TYPE_INPUT) return;
  lf_widget_calc_layout_size(ui, widget);
}

void _do_cursor_blink(
  lf_ui_state_t* ui, 
  lf_timer_t* timer) {
  lf_input_t* input = (lf_input_t*)timer->user_data;
  if(!input->_focused || !lf_widget_in_viewport(ui, &input->base)) {
    return;
  }
  if(input->_blink_held) {
    input->_blink_held = false;
    return;
  }
  ui->needs_render = true;
  input->_show_cursor = !input->_show_cursor; 
}


lf_input_t* 
lf_input_create(
  lf_ui_state_t* ui,
  lf_widget_t* parent,
  char* buf) {
  lf_input_t* input = (lf_input_t*)malloc(sizeof(lf_input_t));
  input->on_focus = NULL;
  input->on_unfocus = NULL;
  input->on_type = NULL;
  input->_hovered = false;
  input->_focused = false;
  input->_blinktimer = NULL;
  input->buf = buf; 

  lf_widget_props_t props = ui->theme->input_props;
  input->base = *lf_widget_create(
    ui->crnt_widget_id++,
    LF_WIDGET_TYPE_INPUT,
    LF_SCALE_CONTAINER(
      parent->container.size.x, 
      20),
    props,
    _input_render, 
    _input_handle_event,
    _input_shape,
    _input_size_calc
  );

  input->_blink_held = false;

  input->base.layout_type = LF_LAYOUT_HORIZONTAL;
  input->base.sizing_type = LF_SIZING_FIT_PARENT;


  lf_widget_listen_for(&input->base, 
                       LF_EVENT_MOUSE_PRESS | LF_EVENT_TYPING_CHAR | LF_EVENT_MOUSE_MOVE);
  
  lf_widget_add_child(parent, (lf_widget_t*)input);

  lf_mapped_font_t font = lf_asset_manager_request_font(
    ui, 
    ui->_ez.last_parent->font_family,
    ui->_ez.last_parent->font_style,
    16
  ); 
  input->_text_widget = lf_text_create_ex(ui, &input->base, buf, font);
  lf_widget_set_padding(ui, &input->_text_widget->base, 0);
  lf_widget_set_margin(ui, &input->_text_widget->base, 0);

  input->_cursor_idx = 0;
  input->_show_cursor = true;

  FT_Face face = font.font->face;
  int font_height_px = 
    (face->size->metrics.ascender - face->size->metrics.descender) / 64;

  lf_widget_set_fixed_height(ui, &input->base, font_height_px + 2);
char esc = 0x1B;
utf8insert(input->buf, 512, strlen(input->buf), &esc, 1);
lf_text_set_label(ui, input->_text_widget, input->buf);
 

  return input;
}
