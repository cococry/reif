#include "../include/leif/widget.h"
#include "../include/leif/ui_core.h"
#include "../include/leif/layout.h"
#include "../include/leif/animation.h"
#include "../include/leif/widgets/text.h"
#include "../include/leif/widgets/div.h"
#include "../include/leif/widgets/slider.h"
#include "../include/leif/widgets/button.h"
#include <stdbool.h>
#include <string.h>
#include <time.h>

#ifdef LF_RUNARA
#include <runara/runara.h>
#endif

#define INIT_CHILD_CAP 4
#define FLOAT_EQ(a, b) (fabsf((a) - (b)) < 0.0001f)

static void widget_resize_children(lf_widget_t* widget, uint32_t new_cap);
static bool widget_animate(lf_ui_state_t* ui, lf_widget_t* widget);


void 
widget_resize_children(lf_widget_t* widget, uint32_t new_cap) {
  if(!widget) return;
  widget->childs = (lf_widget_t**)realloc(widget->childs, new_cap * sizeof(lf_widget_t*));
  widget->cap_childs = new_cap;
}

bool
widget_animate(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(!widget) return false;
  lf_animation_t* prev = NULL;
  lf_animation_t* anim = widget->anims;
 
  bool animated = false;
  while(anim) {
    anim = anim->next;
  }
  anim = widget->anims;
  while (anim) {
    if (anim->active) {
      lf_animation_update(anim, ui->delta_time);
      if(
        anim->target == &widget->props.padding_left || 
        anim->target == &widget->props.padding_right || 
        anim->target == &widget->props.padding_bottom || 
        anim->target == &widget->props.padding_top || 
        anim->target == &widget->props.margin_left || 
        anim->target == &widget->props.margin_right || 
        anim->target == &widget->props.margin_bottom || 
        anim->target == &widget->props.margin_top || 
        anim->target == &widget->container.size.x || 
        anim->target == &widget->container.size.y ||  
        anim->target == &widget->container.pos.x || 
        anim->target == &widget->props.corner_radius_percent || 
        anim->target == &widget->container.pos.y ||  
        anim->target == &widget->_height_percent ||  
        anim->target == &widget->_width_percent 
      ) {
        lf_widget_apply_size_hints(widget);
        widget->_changed_size = true;
        lf_widget_flag_for_layout(ui, widget);
      }
      animated = true;
    }
    if (!anim->active) {
      if (prev) {
        prev->next = anim->next;
      } else {
        widget->anims = anim->next;
      }
      lf_animation_t* to_remove = anim;
      anim = anim->next;
      if(to_remove->keyframes) {
        free(to_remove->keyframes);
        to_remove->keyframes = NULL;
      }
      if(to_remove) {
        free(to_remove);
        to_remove = NULL;
      }
     
      continue;
    }
    prev = anim;
    anim = anim->next;
  }
  return animated;
 }

lf_widget_t* 
lf_widget_create(
  uint32_t id,
  lf_widget_type_t type,
  lf_container_t fallback_container,
  lf_widget_props_t props,
  lf_widget_render_cb render,
  lf_widget_handle_event_cb handle_event,
  lf_widget_shape_cb shape,
  lf_widget_shape_cb size_calc) {
  lf_widget_t* widget = (lf_widget_t*)malloc(sizeof(lf_widget_t));

  widget->parent = NULL;
  widget->cap_childs = INIT_CHILD_CAP;
  widget->num_childs = 0;
  widget->childs = malloc(sizeof(lf_widget_t) * INIT_CHILD_CAP);
  widget->id = id;
  widget->user_data = NULL;
  widget->visible = true;
  widget->rendered = true;
  widget->_marked_for_removal = false;
  widget->_changed_by_layout = false;
  widget->_rendered_within_comp = false;
  widget->_changed_size = true;
  widget->_needs_size_calc = true;
  widget->_needs_shape = true;
  widget->_positioned_absolute_x = false;
  widget->_positioned_absolute_y = false;

  widget->type = type;
  widget->container = fallback_container;
  widget->props = props;
  widget->_rendered_props = props;
  widget->_component_props = props;
  widget->_initial_props = props;
  widget->justify_type = LF_JUSTIFY_START;
  widget->sizing_type = LF_SIZING_FIT_PARENT;

  widget->render = render;
  widget->handle_event = handle_event;
  widget->shape = shape;
  widget->size_calc = size_calc;

  widget->listening_for = 0;
  widget->alignment_flags = 0;

  widget->_fixed_width = false;
  widget->_fixed_height = false;


  widget->anims = NULL; 

  widget->_width_percent = 0.0f;
  widget->_height_percent = 0.0f;
  widget->_abs_x_percent = -1.0f;
  widget->_abs_y_percent = -1.0f;
  
  widget->_min_size = (vec2s){.x = -1.0f, .y = -1.0f};
  widget->_max_size = (vec2s){.x = -1.0f, .y = -1.0f};

  widget->transition_time   = 0.0f;
  widget->transition_func   = NULL;

  widget->scroll_offset = (vec2s){.x = 0, .y = 0};
  
  widget->cull_end_x = 0;
  widget->cull_end_y = 0;
  widget->cull_start_x = 0;
  widget->cull_start_y = 0;

  widget->scrollable = true;
  return widget;
}
void
lf_widget_render(lf_ui_state_t* ui,  lf_widget_t* widget) {
  if(!widget) return;
  if(!widget->visible) return;

  float parent_end_y = 0.0f;
  float parent_start_y = 0.0f;
  float parent_end_x = 0.0f;
  float parent_start_x = 0.0f;

  if (widget->parent) {
    parent_end_y = widget->parent->rendered_pos.y +
      lf_widget_height(widget->parent) - widget->parent->props.padding_bottom;
    if (widget->parent->cull_end_y != 0.0f) {
      parent_end_y = widget->parent->cull_end_y;
    }

    parent_start_y = widget->parent->rendered_pos.y +
      widget->parent->props.padding_top;
    if (widget->parent->cull_start_y != 0.0f) {
      parent_start_y = widget->parent->cull_start_y;
    }

    parent_end_x = widget->parent->rendered_pos.x +
      lf_widget_width(widget->parent) - widget->parent->props.padding_right;
    if (widget->parent->cull_end_x != 0.0f) {
      parent_end_x = widget->parent->cull_end_x;
    }

    parent_start_x = widget->parent->rendered_pos.x +
      widget->parent->props.padding_left;
    if (widget->parent->cull_start_x != 0.0f) {
      parent_start_x = widget->parent->cull_start_x;
    }
  }

  float widget_end_y = widget->rendered_pos.y + lf_widget_height(widget) - widget->props.padding_bottom;
  float widget_start_y = widget->rendered_pos.y + widget->props.padding_top;

  float widget_end_x = widget->rendered_pos.x + lf_widget_width(widget) - widget->props.padding_right;
  float widget_start_x = widget->rendered_pos.x + widget->props.padding_left;

  if(widget->render) {
    if(!lf_widget_in_viewport(ui, widget)) return;
#ifdef LF_RUNARA
    vec2s last_cull_start = ((RnState*)ui->render_state)->cull_start;
    vec2s last_cull_end = ((RnState*)ui->render_state)->cull_end;
    if(widget->parent) {
      rn_set_cull_start_y((RnState*)ui->render_state, parent_start_y);
      rn_set_cull_end_y((RnState*)ui->render_state, parent_end_y);
      rn_set_cull_start_x((RnState*)ui->render_state, parent_start_x);
      rn_set_cull_end_x((RnState*)ui->render_state, parent_end_x );
    }
#endif
    float h = lf_widget_height(widget);
    if(widget->type == LF_WIDGET_TYPE_SLIDER) {
      h =  widget->container.size.y +
        widget->props.padding_top   +
        widget->props.padding_bottom;
    }
    widget->props.corner_radius = h * (widget->props.corner_radius_percent / 100.0f);
    widget->render(ui, widget);
    widget->rendered = true;
#ifdef LF_RUNARA
    if(widget->parent) {
      rn_set_cull_start_y((RnState*)ui->render_state, last_cull_start.y);
      rn_set_cull_end_y( (RnState*)ui->render_state, last_cull_end.y);
      rn_set_cull_start_x((RnState*)ui->render_state,last_cull_start.x);
      rn_set_cull_end_x((RnState*)ui->render_state, last_cull_end.x);
    }
#endif
#ifdef LF_RUNARA
    if(widget->type == LF_WIDGET_TYPE_DIV || widget->type == LF_WIDGET_TYPE_BUTTON) {
      rn_set_cull_start_y(
          (RnState*)ui->render_state,
          widget_start_y > parent_start_y ? widget_start_y : parent_start_y
          );
      rn_set_cull_end_y(
          (RnState*)ui->render_state,
          widget_end_y < parent_end_y ? widget_end_y : parent_end_y
          );

      rn_set_cull_start_x(
          (RnState*)ui->render_state,
          widget_start_x > parent_start_x ? widget_start_x : parent_start_x
          );
      rn_set_cull_end_x(
          (RnState*)ui->render_state,
          (widget_end_x < parent_end_x ? widget_end_x : parent_end_x)
          );

      widget->cull_start_y = widget_start_y > parent_start_y ? widget_start_y : parent_start_y;
      widget->cull_end_y =  widget_end_y < parent_end_y ? widget_end_y : parent_end_y;
      widget->cull_start_x = widget_start_x > parent_start_x ? widget_start_x : parent_start_x ;
      widget->cull_end_x = widget_end_x < parent_end_x ? widget_end_x : parent_end_x;
    }
#endif
  }

  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_render(ui, widget->childs[i]);
  }
  if(widget->type == LF_WIDGET_TYPE_DIV && widget->scrollable) {
    lf_div_t* div = (lf_div_t*)widget;

    vec2s total_scrollable_area = (vec2s){
      .x = widget->total_child_size.x - widget->container.size.x,
      .y = widget->total_child_size.y - widget->container.size.y
    };

    // VERTICAL SCROLLBAR
    if (widget->total_child_size.y > widget->container.size.y) {
      float scroll_progress = -widget->scroll_offset.y / total_scrollable_area.y;
      float scrollbar_height = MAX(
          ui->theme->scrollbar_static_size,
          (widget->container.size.y / widget->total_child_size.y) * widget->container.size.y);

      float scrollbar_y = widget->rendered_pos.y + widget->props.padding_top +
        (scroll_progress * (widget->container.size.y - scrollbar_height));

      lf_container_t scrollbar = (lf_container_t){
        .pos = (vec2s){
          .x = widget->rendered_pos.x + widget->container.size.x + widget->props.padding_left,
          .y = scrollbar_y
        },
        .size = (vec2s){
          .x = ui->theme->scrollbar_static_size,
          .y = scrollbar_height
        },

      };

      div->scrollbars[LF_SCROLLBAR_VERTICAL].container = scrollbar;

      if(lf_container_intersets_container(
            div->scrollbars[LF_SCROLLBAR_VERTICAL].container,
            (lf_container_t){
              .pos = (vec2s){
                .x = widget->parent->rendered_pos.x + widget->props.padding_left,
                .y = widget->parent->rendered_pos.y + widget->props.padding_top
              },
              .size = widget->parent->container.size
            }
      )) {
        rn_set_cull_start_y(
            (RnState*)ui->render_state,
            widget_start_y > parent_start_y ? widget_start_y : parent_start_y
        );
        rn_set_cull_end_y(
            (RnState*)ui->render_state,
            (widget_end_y < parent_end_y ? widget_end_y : parent_end_y)
        );

        rn_set_cull_end_x(
            (RnState*)ui->render_state,
            (widget_end_x < parent_end_x ? widget_end_x : parent_end_x) + widget->props.padding_right
        );
        ui->render_rect(
            ui->render_state,
            scrollbar.pos, scrollbar.size,
            div->scrollbars[LF_SCROLLBAR_VERTICAL].color, ui->theme->scrollbar_props.border_color,
            ui->theme->scrollbar_props.border_width,
            ui->theme->scrollbar_props.corner_radius
        );
      }
    }

    // HORIZONTAL SCROLLBAR
    if (widget->total_child_size.x > widget->container.size.x) {
      float scroll_progress = -widget->scroll_offset.x / total_scrollable_area.x;
      float scrollbar_width = MAX(
          ui->theme->scrollbar_static_size,
          (widget->container.size.x / widget->total_child_size.x) * widget->container.size.x);

      float scrollbar_x = widget->rendered_pos.x + widget->props.padding_left +
        (scroll_progress * (widget->container.size.x - scrollbar_width));

      lf_container_t scrollbar = (lf_container_t){
        .pos = (vec2s){
          .x = scrollbar_x,
          .y = widget->rendered_pos.y + widget->container.size.y + widget->props.padding_top,
        },
        .size = (vec2s){
          .x = scrollbar_width,
          .y = ui->theme->scrollbar_static_size,
        },

      };
      div->scrollbars[LF_SCROLLBAR_HORIZONTAL].container = scrollbar;
      if(lf_container_intersets_container(
          div->scrollbars[LF_SCROLLBAR_HORIZONTAL].container,
          (lf_container_t){
            .pos = (vec2s){
              .x = widget->parent->rendered_pos.x + widget->props.padding_left,
              .y = widget->parent->rendered_pos.y + widget->props.padding_top
            },
            .size = widget->parent->container.size
          }
      )) {
        rn_set_cull_start_y(
            (RnState*)ui->render_state,
            widget_start_y > parent_start_y ? widget_start_y : parent_start_y
        );
        rn_set_cull_end_y(
            (RnState*)ui->render_state,
            (widget_end_y < parent_end_y ? widget_end_y : parent_end_y) + widget->props.padding_top + widget->props.padding_bottom
        );

        rn_set_cull_start_x(
            (RnState*)ui->render_state,
            (widget_start_x > parent_start_x ? widget_start_x : parent_start_x)
        );

        rn_set_cull_end_x(
            (RnState*)ui->render_state,
            (widget_end_x < parent_end_x ? widget_end_x : parent_end_x)
        );

        ui->render_rect(
            ui->render_state,
            scrollbar.pos, scrollbar.size,
            div->scrollbars[LF_SCROLLBAR_HORIZONTAL].color, ui->theme->scrollbar_props.border_color,
            ui->theme->scrollbar_props.border_width,
            ui->theme->scrollbar_props.corner_radius
        );
      }
    }
  }

}

char* printwidgettype(lf_widget_t* widget ){
  switch (widget->type) {
    case LF_WIDGET_TYPE_DIV:
      return ("(Div widget)\n");
      break;
    case LF_WIDGET_TYPE_TEXT: {
      char* buf = malloc(128);
      sprintf(buf, "(Text widget) \'%s\'\n", ((lf_text_t*)(widget))->label);
      return buf; 
    }
    case LF_WIDGET_TYPE_BUTTON: {
      char* buf = malloc(128);
      sprintf(buf, "(Button widget) \'%s\'\n", ((lf_text_t*)widget->childs[0])->label);
      return buf; 
    }
    default: break;
  }
  return "none";
}
static void lf_widget_fit_parent(lf_widget_t* widget) {
  lf_widget_props_t props = widget->props;
  float min_width = -1.0f;
  if(widget->sizing_type == LF_SIZING_FIT_PARENT) {
    if (widget->parent && !widget->_fixed_width) {
      min_width = widget->parent->container.size.x
        - props.padding_left - props.padding_right
        - props.margin_left  - props.margin_right;
      widget->container.size.x = min_width;
    }
  }
}
void lf_widget_size_calc(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(!widget || !ui) return;
  vec2s size_before = widget->container.size;
  for (uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_size_calc(ui, widget->childs[i]);
  }

  if (widget->size_calc && (widget->_needs_size_calc || widget->_changed_size)) {
    widget->size_calc(ui, widget);
    widget->_needs_size_calc = false;
  }

  widget->_changed_size = !(widget->container.size.x == size_before.x &&
    widget->container.size.y == size_before.y);

}
void lf_widget_top_to_bottom_pass(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(!widget || !ui) return;

  vec2s size_before = widget->container.size;

    lf_widget_fit_parent(widget);

  widget->_changed_size = !(widget->container.size.x == size_before.x &&
    widget->container.size.y == size_before.y);

  for (uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_top_to_bottom_pass(ui, widget->childs[i]);
  }
}



void lf_widget_shape(
    lf_ui_state_t* ui,
    lf_widget_t* widget) {
  if (!widget) return;

  bool skip = !widget->_needs_shape;

  if (widget != ui->root && widget->parent && !skip) {
    skip = (!lf_container_intersets_container(
      LF_WIDGET_CONTAINER(widget), ui->root->container) ||
      !lf_container_intersets_container(
        LF_WIDGET_CONTAINER(widget), LF_WIDGET_CONTAINER(widget->parent)));
  }

  if(!skip) skip = widget->shape == NULL;
  if (!skip && (widget->_needs_shape || widget->_changed_size)) {
    widget->shape(ui, widget);
    widget->_needs_shape = false;
  }
  for (uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];
    lf_widget_shape(ui, child);
  }
}

void lf_widget_layout(
  lf_ui_state_t* ui,
  lf_widget_t* widget) {
  if(!widget || !ui) return;
  for(uint32_t i = 0; i < ui->_first_render + 1; i++ ) {
    if(ui->_first_render) {
      lf_widget_invalidate_size_and_layout(ui->root);
    }
    lf_widget_top_to_bottom_pass(ui, widget);
    lf_widget_size_calc(ui, widget);
    lf_widget_shape(ui, widget);
  }
}

bool lf_widget_animate(
  lf_ui_state_t* ui,
  lf_widget_t* widget,
  lf_widget_t** o_shape) {  
  if(!widget) return false;
  bool animated = false;
  if (widget->anims) {
    if(widget_animate(ui, widget)) {
      animated = true;
      if (*o_shape == NULL) {
        *o_shape = widget;
      }
    }
  } 

  for (uint32_t i = 0; i < widget->num_childs; i++) {
    if (lf_widget_animate(ui, widget->childs[i], o_shape)) {
      animated = true;
    }
  }

  return animated;
}


bool 
lf_widget_is_animating(lf_widget_t* widget) {
  if(!widget) return false;
  return widget->anims; 
}

void
lf_widget_handle_event(lf_ui_state_t* ui, lf_widget_t* widget, lf_event_t* event) {
  if(!ui) return;
  if (!widget) return;

  for (uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_handle_event(ui, widget->childs[i], event);
    if (event->handled) return;
  }

  if (!event->handled && widget->handle_event && lf_widget_is_listening(widget, event->type)) {
    widget->handle_event(ui, widget, event); 
  }
}

void
lf_widget_add_child(lf_widget_t* parent, lf_widget_t* child) {
  if(!parent || !child) return;

  child->font_style   = parent->font_style;
  child->font_size    = parent->font_size;
  child->font_family  = parent->font_family;

  if(parent->num_childs >= parent->cap_childs) {
    uint32_t new_cap = parent->cap_childs == 0 ? 2 : parent->cap_childs * 2;
    widget_resize_children(parent, new_cap);
  }
  parent->childs[parent->num_childs++] = child;
  child->parent = parent;
  
  return;
}

void lf_widget_remove(lf_widget_t* widget) {
    if (!widget) return;
    widget->_marked_for_removal = true;

    // Reverse order removal prevents shifting issues
    for (int32_t i = (int32_t)widget->num_childs - 1; i >= 0; i--) {
        lf_widget_remove(widget->childs[i]);
    }
}

void lf_widget_remove_from_memory(lf_widget_t* widget) {
  if (!widget) return;

  for (int32_t i = (int32_t)widget->num_childs - 1; i >= 0; i--) {
    lf_widget_remove_from_memory(widget->childs[i]);
  }

  free(widget->childs);
  widget->childs = NULL;

  if (widget->parent) {
    int32_t child_idx = -1;
    for (uint32_t i = 0; i < widget->parent->num_childs; i++) {
      if (widget->parent->childs[i] == widget) {
        child_idx = i;
        break;
      }
    }

    if (child_idx != -1) {
      widget->parent->num_childs++;
      lf_widget_remove_child_from_memory(widget->parent, child_idx);
    } else {
      fprintf(stderr, "leif: cannot find widget to destroy within parent.\n");
    }
  }

  free(widget);
  widget = NULL;  
}

void lf_widget_remove_child_from_memory(lf_widget_t* parent, uint32_t child_idx) {
  if (!parent || child_idx >= parent->num_childs) return;
  lf_widget_t* child = parent->childs[child_idx];
  free(child);
  child = NULL;

  for (uint32_t i = child_idx; i < parent->num_childs - 1; i++) {
    parent->childs[i] = parent->childs[i + 1];
  }

  parent->num_childs--;

  if (parent->num_childs == 0) {
    free(parent->childs);
    parent->childs = NULL;
    parent->cap_childs = 0;
  } 
}

float 
lf_widget_width(lf_widget_t* widget) {
  if(!widget) return 0.0f;
  return  lf_widget_width_ex(widget, widget->props); 
}

float 
lf_widget_height(lf_widget_t* widget) {
  if(!widget) return 0.0f;
  return lf_widget_height_ex(widget, widget->props); 
}

float 
lf_widget_width_ex(lf_widget_t* widget, lf_widget_props_t props) {
  if(!widget) return 0.0f;
  return widget->container.size.x + 
  props.padding_left      +
  props.padding_right;
}

float 
lf_widget_height_ex(lf_widget_t* widget, lf_widget_props_t props) {
  if(!widget) return 0.0f;
  if(widget->type == LF_WIDGET_TYPE_SLIDER) {
  return ((lf_slider_t*)widget)->handle.size.x + 
    props.padding_top       +
    props.padding_bottom; 
  }
  return widget->container.size.y + 
    props.padding_top       +
    props.padding_bottom;
}

lf_animation_t* lf_widget_set_padding(
  lf_ui_state_t* ui,
  lf_widget_t* widget,
  float padding) {
  if(!widget) return NULL;
  if(
    widget->props.padding_top == padding && 
    widget->props.padding_bottom == padding && 
    widget->props.padding_left == padding && 
    widget->props.padding_right == padding && !widget->transition_func) return NULL;

  lf_widget_set_prop(ui, widget, &widget->props.padding_left, padding); 
  lf_widget_set_prop(ui, widget, &widget->props.padding_right, padding); 
  lf_widget_set_prop(ui, widget, &widget->props.padding_top, padding); 
  lf_animation_t* anim = lf_widget_set_prop(ui, widget, &widget->props.padding_bottom, padding); 

  if(!widget->transition_func && ui->delta_time) {
    ui->needs_render = true;
    widget->_changed_size = true;
    lf_widget_flag_for_layout(ui, widget);
  }

  return anim;
 
}

lf_animation_t* lf_widget_set_margin(
  lf_ui_state_t* ui,
  lf_widget_t* widget,
  float margin) {
  if(!widget) return NULL;
  if(
    widget->props.margin_top == margin && 
    widget->props.margin_bottom == margin && 
    widget->props.margin_left == margin && 
    widget->props.margin_right == margin && !widget->transition_func) return NULL;

  lf_widget_set_prop(ui, widget, &widget->props.margin_left, margin); 
  lf_widget_set_prop(ui, widget, &widget->props.margin_right, margin); 
  lf_widget_set_prop(ui, widget, &widget->props.margin_top, margin); 
  lf_animation_t* anim = lf_widget_set_prop(ui, widget, &widget->props.margin_bottom, margin); 

  widget->_changed_size = true;
  if(!widget->transition_func) {
    ui->needs_render = true;
    lf_widget_flag_for_layout(ui, widget);
  }

  return anim;
}

void 
lf_widget_set_layout(lf_widget_t* widget, lf_layout_type_t layout) {
  if(!widget) return;
  widget->layout_type = layout;
}

void 
lf_widget_apply_layout(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(!widget) return;
  switch (widget->layout_type) {
    case LF_LAYOUT_VERTICAL:
      lf_layout_vertical(ui, widget);
      break;
    case LF_LAYOUT_HORIZONTAL:
      lf_layout_horizontal(ui, widget);
      break;
    case LF_LAYOUT_GRID:
      lf_layout_responsive_grid(ui, widget);
      break;
    default:
        break;
  }
}

void 
lf_widget_calc_layout_size(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(!widget) return;
  switch (widget->layout_type) {
    case LF_LAYOUT_VERTICAL: 
      lf_size_calc_vertical(ui, widget);
      break;
    case LF_LAYOUT_HORIZONTAL:
      lf_size_calc_horizontal(ui, widget);
      break;
    case LF_LAYOUT_GRID:
      break;
    default:
        break;
  }
}

void 
lf_widget_listen_for(lf_widget_t* widget, uint32_t events) {
  if(!widget) return;
  widget->listening_for |= events;
}

void 
lf_widget_unlisten(lf_widget_t* widget, uint32_t events) {
  if(!widget) return;
  widget->listening_for &= ~events;
}

bool 
lf_widget_is_listening(lf_widget_t* widget, uint32_t events) {
  if(!widget) return false;
  return (widget->listening_for & events) != 0;
}

lf_widget_t*
lf_widget_from_id(lf_ui_state_t* ui, lf_widget_t* root, uint32_t id) {
  if (root->id == id) return root;
  for (uint32_t i = 0; i < root->num_childs; i++) {
    lf_widget_t* widget = lf_widget_from_id(ui, root->childs[i], id);
    if (widget != NULL) return widget; 
  }
  return NULL; // Not found in this subtree
}


void 
lf_widget_set_listener(lf_widget_t* widget, lf_widget_handle_event_cb cb, uint32_t events) {
  if(!widget) return;
  widget->handle_event = cb;
  lf_widget_listen_for(widget, events);
}

lf_animation_t* lf_widget_add_animation(
  lf_widget_t* widget,
  float *target, 
  float start_value, 
  float end_value, 
  float duration,
  lf_animation_func_t func) {
  if(!widget) return NULL;
  lf_widget_interrupt_animation(widget, target);
  return lf_animation_create(
    &widget->anims,
    target, (lf_animation_keyframe_t[]){
      (lf_animation_keyframe_t){
        .start = start_value,
        .end = end_value, 
        .duration = duration,
        .easing = func
      }
    }, 1, false
  );
}

lf_animation_t* lf_widget_add_animation_looped(
    lf_widget_t* widget,
    float *target, 
    float start_value, 
    float end_value, 
    float duration,
    lf_animation_func_t func) {
  if(!widget) return NULL;
  lf_widget_interrupt_animation(widget, target);
  return lf_animation_create(
    &widget->anims,
    target, (lf_animation_keyframe_t[]){
      (lf_animation_keyframe_t){
        .start = start_value,
        .end = end_value, 
        .duration = duration,
        .easing = func
      }
    }, 1, true 
  );
}


lf_animation_t* lf_widget_add_keyframe_animation(
    lf_widget_t* widget,
    float *target,
    lf_animation_keyframe_t* keyframes,
    uint32_t n_keyframes) {
  if(!widget) return NULL;
  lf_widget_interrupt_animation(widget, target);
  return lf_animation_create(
    &widget->anims,
    target,
    keyframes,
    n_keyframes,
    false
  );
}

lf_animation_t* lf_widget_add_keyframe_animation_looped(
    lf_widget_t* widget,
    float *target,
    lf_animation_keyframe_t* keyframes,
    uint32_t n_keyframes) {
  if(!widget) return NULL;
  lf_widget_interrupt_animation(widget, target);
  return lf_animation_create(
    &widget->anims,
    target,
    keyframes,
    n_keyframes,
    true 
  );
}

void lf_widget_interrupt_animation(
    lf_widget_t* widget,
    float *target) {
  if(!widget) return;
  lf_animation_interrupt(widget->anims, target);
}

void lf_widget_interrupt_all_animations(
    lf_widget_t* widget) {
  if(!widget) return;
  lf_animation_t* anim = widget->anims;
  while(anim) {
    anim->active = false;
    anim = anim->next;
  }
}

void 
lf_widget_submit_props(lf_widget_t* widget) {
  if(!widget) return;
  widget->_rendered_props = widget->props;
}

lf_animation_t*
lf_widget_set_fixed_width(lf_ui_state_t* ui, lf_widget_t* widget, float width) {
  if(!widget) return NULL;
  if(widget->container.size.x == width) return NULL;
  lf_animation_t* anim = lf_widget_set_prop(ui, widget, &widget->container.size.x, width);
  widget->_fixed_width = true;

  if(!widget->transition_func && ui->delta_time) {
    ui->needs_render = true;
    widget->_changed_size = true;
    lf_widget_flag_for_layout(ui, widget);
  }
  return anim; 

}

lf_animation_t* 
lf_widget_set_fixed_height(lf_ui_state_t* ui, lf_widget_t* widget, float height) {
  if(!widget) return NULL;
  if(widget->container.size.y == height) return NULL;
  lf_animation_t* anim = lf_widget_set_prop(ui, widget, &widget->container.size.y, height);
  widget->_fixed_height = true;
  if(!widget->anims && ui->delta_time) {
    widget->_changed_size = true;
    ui->needs_render = true;
    lf_widget_flag_for_layout(ui, widget);
  }
  return anim;
}

lf_animation_t* 
lf_widget_set_fixed_width_percent(lf_ui_state_t* ui, lf_widget_t* widget, float percent) {
  if(!widget) return NULL;
  lf_animation_t* anim = 
    lf_widget_set_prop(ui, widget, &widget->_height_percent, percent / 100.0f);
  widget->_fixed_width = true;
  widget->_changed_size = true;
  if(!widget->anims)
    lf_widget_flag_for_layout(ui, widget);
  return anim;
}

lf_animation_t*
lf_widget_set_fixed_height_percent(lf_ui_state_t* ui, lf_widget_t* widget, float percent) {
  if(!widget) return NULL;
  lf_animation_t* anim = 
    lf_widget_set_prop(ui, widget, &widget->_height_percent, percent / 100.0f);
  widget->_fixed_height = true;
  widget->_changed_size = true;
  if(!widget->anims)
    lf_widget_flag_for_layout(ui, widget);
  return anim;
}

void lf_widget_set_alignment(lf_widget_t* widget, uint32_t flags) {
  if(!widget) return;
  lf_flag_set(&widget->alignment_flags, flags);
}

void 
lf_widget_set_sizing(lf_widget_t* widget, lf_sizing_type_t sizing) {
  if(!widget) return;
  widget->sizing_type = sizing;
}

void widget_handle_max_size(lf_widget_t *widget) {
  if (!widget || !widget->parent) return;
  lf_widget_t* parent = widget->parent;

  // Horizontal (x) handling
  float available_width = parent->container.size.x;
  int max_sized_count_x = 0;
  float used_width = 0.0f;

  // Vertical (y) handling
  float available_height = parent->container.size.y;
  int max_sized_count_y = 0;
  float used_height = 0.0f;

  for (int i = 0; i < parent->num_childs; ++i) {
    lf_widget_t *child = parent->childs[i];

    // Horizontal
    if (child->_max_size.x != -1.0f) {
      max_sized_count_x++;
      used_width += child->_max_size.x +
        child->props.padding_left + child->props.padding_right +
        child->props.margin_left + child->props.margin_right;
    } else {
      used_width += lf_widget_width(child) +
        child->props.margin_left + child->props.margin_right;
    }

    // Vertical
    if (child->_max_size.y != -1.0f) {
      max_sized_count_y++;
      used_height += child->_max_size.y +
        child->props.padding_top + child->props.padding_bottom +
        child->props.margin_top + child->props.margin_bottom;
    } else {
      used_height += lf_widget_height(child) +
        child->props.margin_top + child->props.margin_bottom;
    }
  }

  if(widget->parent->layout_type == LF_LAYOUT_VERTICAL) {
    if(widget->_max_size.x != -1.0f) {
      widget->container.size.x = widget->container.size.x > 
        widget->_max_size.x ? widget->_max_size.x : widget->container.size.x; 
    }
    if(widget->_max_size.y != -1.0f) {
      widget->container.size.y = widget->container.size.y > 
        widget->_max_size.y ? widget->_max_size.y : widget->container.size.y; 
    }
  } else {
    // Horizontal overflow
    float overflow_x = used_width - available_width;
    if (overflow_x > 0.0f && max_sized_count_x > 0) {
      float reduction_per_widget_x = overflow_x / max_sized_count_x;
      if (widget->_max_size.x != -1.0f) {
        float new_width = widget->_max_size.x - reduction_per_widget_x;
        if (new_width < 0.0f) new_width = 0.0f;
        widget->container.size.x = new_width;
      }
    } else {
      if (widget->_max_size.x != -1.0f) {
        widget->container.size.x = widget->_max_size.x;
      }
    }

  // Vertical overflow
  float overflow_y = used_height - available_height;
  if (overflow_y > 0.0f && max_sized_count_y > 0) {
    float reduction_per_widget_y = overflow_y / max_sized_count_y;
    if (widget->_max_size.y != -1.0f) {
      float new_height = widget->_max_size.y - reduction_per_widget_y;
      if (new_height < 0.0f) new_height = 0.0f;
      widget->container.size.y = new_height;
    }
  } else {
    if (widget->_max_size.y != -1.0f) {
      widget->container.size.y = widget->_max_size.y;
    }
  }
  }
}

void 
lf_widget_apply_size_hints(lf_widget_t* widget) {
  if(widget->_min_size.x != -1.0f) {
    widget->container.size.x = widget->container.size.x <
      widget->_min_size.x ? widget->_min_size.x : widget->container.size.x; 
  }
 
  if(widget->_min_size.y != -1.0f) {
    widget->container.size.y = widget->container.size.y < 
      widget->_min_size.y ? widget->_min_size.y :  widget->container.size.y; 
  }
  widget_handle_max_size(widget);

  if (widget->_fixed_width && widget->_width_percent != 0.0f && widget->parent) {
    widget->container.size.x = lf_widget_width(widget->parent) * 
      widget->_width_percent - 
      widget->props.padding_left - widget->props.padding_right - 
      widget->parent->props.padding_left - widget->parent->props.padding_right -
    widget->props.margin_left - widget->props.margin_right
    ;
  }
  if (widget->_fixed_height && widget->_height_percent != 0.0f && widget->parent) {
    widget->container.size.y = MAX(0, lf_widget_height(widget->parent) * 
      widget->_height_percent - 
      widget->props.padding_top - widget->props.padding_bottom - 
      widget->parent->props.padding_top - widget->parent->props.padding_bottom - 
      widget->props.margin_top - widget->props.margin_bottom);
    ; 
  }
}

void 
lf_widget_set_min_width(lf_widget_t* widget, float width) {
  widget->_min_size.x = width;
  lf_widget_apply_size_hints(widget);
}

void 
lf_widget_set_max_width(lf_widget_t* widget, float width) {
  widget->_max_size.x = width;
  lf_widget_apply_size_hints(widget);
}

void 
lf_widget_set_min_height(lf_widget_t* widget, float height) {
  widget->_min_size.y = height;
  lf_widget_apply_size_hints(widget);
}

void 
lf_widget_set_max_height(lf_widget_t* widget, float height) {
  widget->_max_size.y = height;
  lf_widget_apply_size_hints(widget);
}

void 
lf_widget_set_font_style(lf_ui_state_t* ui, lf_widget_t* widget, lf_font_style_t style) {
  if(widget->type == LF_WIDGET_TYPE_TEXT) {
    widget->font_style = style;
    lf_text_set_font(ui, (lf_text_t*)widget, widget->font_family, style, ((lf_text_t*)widget)->font.pixel_size);
  }
}

void 
lf_widget_set_font_family(lf_ui_state_t* ui, lf_widget_t* widget, const char* font_family) {
  widget->font_family = font_family;
  if(widget->type == LF_WIDGET_TYPE_TEXT) {
    lf_text_set_font(ui, (lf_text_t*)widget, font_family, widget->font_style, ((lf_text_t*)widget)->font.pixel_size);
  }
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_set_font_family(ui, widget->childs[i], font_family);
  }
}

void 
lf_widget_set_font_size(lf_ui_state_t* ui, lf_widget_t* widget, uint32_t pixel_size) {
  widget->font_size = pixel_size;
  if(widget->type == LF_WIDGET_TYPE_TEXT) {
    lf_text_set_font(ui, (lf_text_t*)widget, widget->font_family, widget->font_style, pixel_size); 
  }
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_set_font_size(ui, widget->childs[i], pixel_size);
  }
}

vec2s 
lf_widget_measure_children_wrapped(lf_ui_state_t* ui, lf_widget_t* widget, vec2s* o_max) {
  if (!widget || !widget->visible) return (vec2s){.x = 0, .y = 0};

  lf_widget_props_t widget_props = widget->props;

  float container_left = widget->container.pos.x + widget_props.padding_left;
  float container_right = widget->container.pos.x + widget->container.size.x - widget_props.padding_right;
  float container_width = container_right - container_left;

  float line_width = 0.0f;
  float line_height = 0.0f;
  float max_width = 0.0f;
  float total_height = 0.0f;
  bool nowrapping = false;

  vec2s max_child = {.x = 0, .y = 0};

  for (uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];
    if (!child || !child->visible) continue;

    if(child->_needs_size_calc && child->size_calc) {
      child->size_calc(ui, child);
    }
    vec2s size = LF_WIDGET_SIZE_V2(child);
    if(child->sizing_type == LF_SIZING_GROW) nowrapping = true;
    float margin_left = child->props.margin_left;
    float margin_right = child->props.margin_right;
    float margin_top = child->props.margin_top;
    float margin_bottom = child->props.margin_bottom;

    float child_total_width = margin_left + size.x + margin_right;
    float child_total_height = margin_top + size.y + margin_bottom;
    if(!nowrapping) {
      if (line_width > 0 && (line_width - margin_right + lf_widget_width(child)) > container_width) {
        if (line_width > max_width) max_width = line_width;
        total_height += line_height;

        line_width = 0.0f;
        line_height = 0.0f;
      }
    }

    line_width += child_total_width;
    if (child_total_height > line_height) {
      line_height = child_total_height;
    }

    if(child->sizing_type != LF_SIZING_GROW) {
      if (child->container.size.x > max_child.x) max_child.x = lf_widget_effective_size(child).x + child->props.margin_left; 
      if (child->container.size.y > max_child.y) max_child.y = lf_widget_effective_size(child).y; 
    }
  }

  // Add last line if there were children on it
  if (line_width > 0 || total_height == 0.0f) {
    if (line_width > max_width) max_width = line_width;
    total_height += line_height;
  }


  if (o_max) *o_max = max_child;

  return (vec2s){ .x = max_width, .y = total_height };
}


vec2s 
lf_widget_measure_children(lf_widget_t* widget, vec2s* o_max) {
  vec2s size = (vec2s){
    .x = 0,
    .y = 0
  };
  vec2s max = (vec2s){
    .x = 0,
    .y = 0
  };

  vec2s ptr = (vec2s){
    .x = widget->container.pos.x,
    .y = widget->container.pos.y 
  };
  vec2s ptr_before = ptr;
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];
    if (!child->visible) continue;

    vec2s size = lf_widget_effective_size(child);
    ptr.x += size.x; 
    ptr.y += size.y; 

    if(size.x > max.x) {
      max.x = size.x; 
    }
    if(size.y > max.y) {
      max.y = size.y; 
    }
  }

  size.x = ptr.x - ptr_before.x; 
  size.y = ptr.y - ptr_before.y;

  widget->total_child_size = size;

  if(o_max) {
    *o_max = max;
  }

  return size;

}

vec2s
lf_widget_effective_size(lf_widget_t* widget) {
  return (vec2s){
    .x = lf_widget_width(widget) + 
    widget->props.margin_right + widget->props.margin_left,
    .y = lf_widget_height(widget) + 
    widget->props.margin_top + widget->props.margin_bottom
  };
}

void
lf_widget_set_transition_props(
    lf_widget_t* widget, float transition_time,
    lf_animation_func_t transition_func) {
  widget->transition_time = transition_time;
  widget->transition_func = transition_func;
}

lf_animation_t* lf_widget_set_prop(
  lf_ui_state_t* ui,
  lf_widget_t* widget, 
  float* prop, float val) {
  if(*prop == val) return NULL;
  if(widget->transition_func && ui->delta_time != 0.0f) {
    return lf_widget_add_animation(
      widget,
      prop, 
      *prop, val,
      widget->transition_time, widget->transition_func); 
  } else {
    *prop = val;
    return NULL;
  }
}

void lf_widget_set_props(
    lf_ui_state_t* ui,
    lf_widget_t* widget,
    lf_widget_props_t props) {
  if (!widget) return;

  vec2s sizebefore = LF_WIDGET_SIZE_RENDERED_V2(widget);
  // Padding
  lf_widget_set_prop(ui, widget, &widget->props.padding_left, props.padding_left);
  lf_widget_set_prop(ui, widget, &widget->props.padding_right, props.padding_right);
  lf_widget_set_prop(ui, widget, &widget->props.padding_top, props.padding_top);
  lf_widget_set_prop(ui, widget, &widget->props.padding_bottom, props.padding_bottom);

  // Margin
  lf_widget_set_prop(ui, widget, &widget->props.margin_left, props.margin_left);
  lf_widget_set_prop(ui, widget, &widget->props.margin_right, props.margin_right);
  lf_widget_set_prop(ui, widget, &widget->props.margin_top, props.margin_top);
  lf_widget_set_prop(ui, widget, &widget->props.margin_bottom, props.margin_bottom);

  // Radius, border width, radius percent
  lf_widget_set_prop(ui, widget, &widget->props.corner_radius_percent, props.corner_radius_percent);
  lf_widget_set_prop(ui, widget, &widget->props.border_width, props.border_width);

  // Colors
  lf_widget_set_prop_color(ui, widget, &widget->props.color, props.color);
  lf_widget_set_prop_color(ui, widget, &widget->props.text_color, props.text_color);
  lf_widget_set_prop_color(ui, widget, &widget->props.border_color, props.border_color);

  widget->props.text_align = props.text_align;

  // Trigger layout/size update if not animated
  if (!widget->transition_func) {
    ui->needs_render = true;
    if(
      sizebefore.x != LF_WIDGET_SIZE_RENDERED_V2(widget).x || 
      sizebefore.y != LF_WIDGET_SIZE_RENDERED_V2(widget).y  
    ) {
      widget->_changed_size = true;
    }
    lf_widget_flag_for_layout(ui, widget);
  }
}


lf_animation_t* 
lf_widget_set_prop_color(
  lf_ui_state_t* ui,
  lf_widget_t* widget, 
  lf_color_t* prop, lf_color_t val) {
  lf_widget_set_prop(ui, widget, &prop->r, val.r);
  lf_widget_set_prop(ui, widget, &prop->g, val.g);
  lf_widget_set_prop(ui, widget, &prop->b, val.b);
  lf_animation_t* anim = lf_widget_set_prop(ui, widget, &prop->a, val.a);
  ui->needs_render = true;
  return anim;
}

void 
lf_widget_set_visible(lf_ui_state_t* ui, lf_widget_t* widget, bool visible) {
  widget->visible = visible;
  widget->_changed_size = true;
  lf_widget_flag_for_layout(ui, widget);
}

void 
lf_widget_invalidate_size(lf_widget_t* widget) {
  widget->_needs_size_calc = true;
  for (size_t i = 0; i < widget->num_childs; i++) {
    lf_widget_invalidate_size(widget->childs[i]);
  }
}

void 
lf_widget_invalidate_layout(lf_widget_t* widget) {
  widget->_needs_shape = true;
  for (size_t i = 0; i < widget->num_childs; i++) {
    lf_widget_invalidate_layout(widget->childs[i]);
  }
}

void 
lf_widget_invalidate_size_and_layout(lf_widget_t* widget) {
  widget->_needs_size_calc = true;
  widget->_needs_shape = true;
  for (size_t i = 0; i < widget->num_childs; i++) {
    lf_widget_invalidate_size_and_layout(widget->childs[i]);
  }
}

lf_widget_t*
lf_widget_flag_for_layout(lf_ui_state_t* ui, lf_widget_t* widget) {
  if (!widget || !widget->size_calc) return NULL;

  lf_widget_t* to_flag = widget;
  bool size_changed = false;

  while (to_flag) {
    vec2s size_before;
    if(!to_flag->_changed_size) {
      size_before = to_flag->container.size;
      to_flag->size_calc(ui, to_flag); 
    }

    if ((to_flag->_changed_size || (
      to_flag->container.size.x != size_before.x || 
      to_flag->container.size.y != size_before.y) ||
      to_flag->sizing_type == LF_SIZING_FIT_CONTENT) && to_flag->parent) {
      size_changed = true;
      to_flag = to_flag->parent;
    } else {
      break;
    }
  }

  if (size_changed && to_flag) {
    lf_widget_invalidate_size_and_layout(to_flag);
    return to_flag;
  }
  return NULL;
}

void 
lf_widget_set_pos_x(lf_widget_t* widget, float pos) {
  if(!widget || widget->_positioned_absolute_x) return;
  widget->container.pos.x = pos;
  widget->rendered_pos.x = widget->container.pos.x +  widget->parent->scroll_offset.x;
  widget->scroll_offset.x = widget->parent->scroll_offset.x;
}

void lf_widget_set_pos_y(lf_widget_t* widget, float pos) {
  if(!widget || widget->_positioned_absolute_y) return;
  widget->container.pos.y = pos; 
  widget->rendered_pos.y = widget->container.pos.y +  widget->parent->scroll_offset.y;
  widget->scroll_offset.y = widget->parent->scroll_offset.y;
}

bool lf_widget_or_childs_changed_size(
    lf_widget_t* widget) {
  if(widget->_changed_size) return true;
  for (size_t i = 0; i < widget->num_childs; i++) {
    if(lf_widget_or_childs_changed_size(widget->childs[i])) return true;
  }
  return false;
}


void 
lf_widget_set_pos_x_absolute(lf_widget_t* widget, float x) {
  if(!widget) return;
  if(widget->parent)
    widget->parent->scrollable = false;
  widget->_positioned_absolute_x = true;
  widget->container.pos.x = x;
}

void 
lf_widget_set_pos_y_absolute(lf_widget_t* widget, float y) {
  if(!widget) return;
  if(widget->parent)
    widget->parent->scrollable = false;
  widget->_positioned_absolute_y = true;
  widget->container.pos.y = y;
}

void 
lf_widget_set_pos_x_absolute_percent(lf_widget_t* widget, float x) {
  if(!widget) return;
  if(widget->parent)
    widget->parent->scrollable = false;
  widget->_positioned_absolute_x = true;
  widget->_abs_x_percent = x;
}

void 
lf_widget_set_pos_y_absolute_percent(lf_widget_t* widget, float y) {
  if(!widget) return;
  if(widget->parent)
    widget->parent->scrollable = false;
  widget->_positioned_absolute_y = true;
  widget->_abs_y_percent = y;
}

bool 
lf_widget_in_viewport(lf_ui_state_t* ui, lf_widget_t* widget) {
    if(!lf_container_intersets_container(
          LF_WIDGET_CONTAINER(widget), ui->root->container) || 
        !lf_container_intersets_container(
          LF_WIDGET_CONTAINER(widget), LF_WIDGET_CONTAINER(widget->parent))) {
      return false;
    }
  return true;
}

bool lf_color_equal(lf_color_t a, lf_color_t b);

bool lf_widget_props_equal(lf_widget_props_t a, lf_widget_props_t b) {
  return
    lf_color_equal(a.color, b.color) &&
    lf_color_equal(a.text_color, b.text_color) &&

    FLOAT_EQ(a.padding_left, b.padding_left) &&
    FLOAT_EQ(a.padding_right, b.padding_right) &&
    FLOAT_EQ(a.padding_top, b.padding_top) &&
    FLOAT_EQ(a.padding_bottom, b.padding_bottom) &&

    FLOAT_EQ(a.margin_left, b.margin_left) &&
    FLOAT_EQ(a.margin_right, b.margin_right) &&
    FLOAT_EQ(a.margin_top, b.margin_top) &&
    FLOAT_EQ(a.margin_bottom, b.margin_bottom) &&

    FLOAT_EQ(a.corner_radius, b.corner_radius) &&
    FLOAT_EQ(a.border_width, b.border_width) &&
    FLOAT_EQ(a.corner_radius_percent, b.corner_radius_percent) &&

    lf_color_equal(a.border_color, b.border_color) &&

    a.text_align == b.text_align;
}
