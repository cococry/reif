#include "../include/leif/widget.h"
#include "../include/leif/ui_core.h"
#include "../include/leif/layout.h"
#include "../include/leif/animation.h"
#include "../include/leif/widgets/text.h"
#include "../include/leif/widgets/div.h"
#include <string.h>
#include <time.h>

#ifdef LF_RUNARA
#include <runara/runara.h>
#endif

#define INIT_CHILD_CAP 4

static void widget_resize_children(lf_widget_t* widget, uint32_t new_cap);
static void widget_animate(lf_ui_state_t* ui, lf_widget_t* widget);


void 
widget_resize_children(lf_widget_t* widget, uint32_t new_cap) {
  if(!widget) return;
  widget->childs = (lf_widget_t**)realloc(widget->childs, new_cap * sizeof(lf_widget_t*));
  widget->cap_childs = new_cap;
}

void 
widget_animate(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(!widget) return;
  lf_animation_t* prev = NULL;
  lf_animation_t* anim = widget->anims;

  while (anim) {
    if (anim->active) {
      lf_animation_update(anim, ui->delta_time);
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
  widget->visible = true;
  widget->_marked_for_removal = false;
  widget->_changed_by_layout = false;
  widget->_rendered_within_comp = false;
  widget->_changed_size = true;
  widget->_needs_size_calc = true;
  widget->_needs_shape = true;

  widget->type = type;
  widget->container = fallback_container;
  widget->props = props;
  widget->_rendered_props = props;
  widget->_initial_props = props;
  widget->justify_type = JustifyStart;
  widget->sizing_type = SizingFitToParent;

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
  
  widget->_min_size = (vec2s){.x = -1.0f, .y = -1.0f};
  widget->_max_size = (vec2s){.x = -1.0f, .y = -1.0f};

  widget->transition_time   = 0.0f;
  widget->transition_func   = NULL;

  widget->scroll_offset = (vec2s){.x = 0, .y = 0};

  return widget;
}


void
lf_widget_render(lf_ui_state_t* ui,  lf_widget_t* widget) {
  if(!widget) return;
  if(!widget->visible) return;
  if(widget->render) {
    if(!lf_container_intersets_container(
      LF_WIDGET_CONTAINER(widget), ui->root->container) || 
      !lf_container_intersets_container(
        LF_WIDGET_CONTAINER(widget), LF_WIDGET_CONTAINER(widget->parent))) {
      return;
    }

    float parent_end_y = widget->parent->container.pos.y + 
      lf_widget_height(widget->parent) - widget->parent->props.padding_bottom; 
    float parent_start_y = widget->parent->container.pos.y +
      widget->parent->props.padding_top; 

    float parent_end_x = widget->parent->container.pos.x + 
      lf_widget_width(widget->parent) - widget->parent->props.padding_right; 
    float parent_start_x = widget->parent->container.pos.x +
      widget->parent->props.padding_left;

#ifdef LF_RUNARA
    if(widget->type == WidgetTypeDiv) {
      rn_set_cull_start_y(
        (RnState*)ui->render_state,
        parent_start_y
      );
      rn_set_cull_end_y(
        (RnState*)ui->render_state,
        parent_end_y
      );
      rn_set_cull_start_x(
        (RnState*)ui->render_state,
        parent_start_x
      );
      rn_set_cull_end_x(
        (RnState*)ui->render_state,
        parent_end_x
      );
    }
#endif
    widget->render(ui, widget);
#ifdef LF_RUNARA
    if(widget->type == WidgetTypeDiv) {

      float widget_end_y = widget->container.pos.y + lf_widget_height(widget) - widget->props.padding_bottom;
      float widget_start_y = widget->container.pos.y + widget->props.padding_top; 
      
      float widget_end_x = widget->container.pos.x + lf_widget_width(widget) - widget->props.padding_right;
      float widget_start_x = widget->container.pos.x + widget->props.padding_left; 

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
        widget_end_x < parent_end_x ? widget_end_x : parent_end_x 
      );

    }
#endif
  }    

  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_render(ui, widget->childs[i]);
  }
  if(widget->type == WidgetTypeDiv) {
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

      float scrollbar_y = widget->container.pos.y + widget->props.padding_top + 
        (scroll_progress * (widget->container.size.y - scrollbar_height));

      lf_container_t scrollbar = (lf_container_t){
        .pos = (vec2s){
          .x = widget->container.pos.x + widget->container.size.x + widget->props.padding_left, 
          .y = scrollbar_y
        },
        .size = (vec2s){
          .x = ui->theme->scrollbar_static_size,
          .y = scrollbar_height
        },

      };

      div->scrollbars[LF_SCROLLBAR_VERTICAL].container = scrollbar;

      vec2s cull_before_start = ((RnState*)ui->render_state)->cull_start;
      vec2s cull_before_end = ((RnState*)ui->render_state)->cull_end;
      rn_unset_cull_end_y(ui->render_state);
      rn_unset_cull_end_x(ui->render_state);
      rn_unset_cull_start_y(ui->render_state);
      rn_unset_cull_start_x(ui->render_state);
      ui->render_rect(
        ui->render_state,
        scrollbar.pos, scrollbar.size,
        div->scrollbars[LF_SCROLLBAR_VERTICAL].color, ui->theme->scrollbar_props.border_color,
        ui->theme->scrollbar_props.border_width, 
        ui->theme->scrollbar_props.corner_radius
      );
      rn_set_cull_start_x(ui->render_state, cull_before_start.x);
      rn_set_cull_start_y(ui->render_state, cull_before_start.y);
      rn_set_cull_end_x(ui->render_state, cull_before_end.x);
      rn_set_cull_end_y(ui->render_state, cull_before_end.y);
    }

    // HORIZONTAL SCROLLBAR
    if (widget->total_child_size.x > widget->container.size.x) {
      float scroll_progress = -widget->scroll_offset.x / total_scrollable_area.x;
      float scrollbar_width = MAX(
        ui->theme->scrollbar_static_size, 
        (widget->container.size.x / widget->total_child_size.x) * widget->container.size.x);

      float scrollbar_x = widget->container.pos.x + widget->props.padding_left + 
        (scroll_progress * (widget->container.size.x - scrollbar_width));

      lf_container_t scrollbar = (lf_container_t){
        .pos = (vec2s){
          .x = scrollbar_x,
          .y = widget->container.pos.y + widget->container.size.y + widget->props.padding_top, 
        },
        .size = (vec2s){
          .x = scrollbar_width,
          .y = ui->theme->scrollbar_static_size,
        },

      };

      div->scrollbars[LF_SCROLLBAR_HORIZONTAL].container = scrollbar;

      vec2s cull_before_start = ((RnState*)ui->render_state)->cull_start;
      vec2s cull_before_end = ((RnState*)ui->render_state)->cull_end;
      rn_unset_cull_end_y(ui->render_state);
      rn_unset_cull_end_x(ui->render_state);
      rn_unset_cull_start_y(ui->render_state);
      rn_unset_cull_start_x(ui->render_state);
      ui->render_rect(
        ui->render_state,
        scrollbar.pos, scrollbar.size,
        div->scrollbars[LF_SCROLLBAR_HORIZONTAL].color, ui->theme->scrollbar_props.border_color,
        ui->theme->scrollbar_props.border_width, 
        ui->theme->scrollbar_props.corner_radius
      );
      rn_set_cull_start_x(ui->render_state, cull_before_start.x);
      rn_set_cull_start_y(ui->render_state, cull_before_start.y);
      rn_set_cull_end_x(ui->render_state, cull_before_end.x);
      rn_set_cull_end_y(ui->render_state, cull_before_end.y);
    }




  }

}

void lf_widget_shape(lf_ui_state_t* ui, lf_widget_t* widget) {
  if (!widget || !widget->shape || !widget->_needs_shape) return;

  if (widget != ui->root && widget->parent) {
    if (!lf_container_intersets_container(
          LF_WIDGET_CONTAINER(widget), ui->root->container) ||
        !lf_container_intersets_container(
          LF_WIDGET_CONTAINER(widget), LF_WIDGET_CONTAINER(widget->parent))) {
      return;
    }
  }
  vec2s size_before = widget->container.size;

  if (widget->size_calc){
    widget->size_calc(ui, widget);
  }

  widget->_changed_size = !(widget->container.size.x == size_before.x &&
                            widget->container.size.y == size_before.y);

  widget->shape(ui, widget);
  widget->_needs_shape = false;

  for (uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_shape(ui, widget->childs[i]);
  }
}


bool lf_widget_animate(
  lf_ui_state_t* ui,
  lf_widget_t* widget,
  lf_widget_t** o_shape) {  
  if(!widget) return false;
  bool animated = false;
  if (widget->anims) {
    widget_animate(ui, widget);
    widget->_changed_size = true;
    lf_widget_submit_props(widget);
    lf_widget_shape(ui, lf_widget_flag_for_layout(ui, widget));
    animated = true;
    if (*o_shape == NULL) {
      *o_shape = widget;
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
lf_widget_handle_event(lf_ui_state_t* ui, lf_widget_t* widget, lf_event_t event) {
  if(!widget) return;
  if(widget->handle_event && lf_widget_is_listening(widget, event.type)) {
    widget->handle_event(ui, widget, event); 
  }
  if(!widget) return;
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_handle_event(ui, widget->childs[i], event);
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
  return widget->container.size.y + 
    props.padding_top       +
    props.padding_bottom;
}

void lf_widget_set_padding(
  lf_ui_state_t* ui,
  lf_widget_t* widget,
  float padding) {
  if(!widget) return;
  if(
    widget->props.padding_top == padding && 
    widget->props.padding_bottom == padding && 
    widget->props.padding_left == padding && 
    widget->props.padding_right == padding) return;

  lf_widget_set_prop(ui, widget, &widget->props.padding_left, padding); 
  lf_widget_set_prop(ui, widget, &widget->props.padding_right, padding); 
  lf_widget_set_prop(ui, widget, &widget->props.padding_top, padding); 
  lf_widget_set_prop(ui, widget, &widget->props.padding_bottom, padding); 
  widget->_changed_size = true;
  lf_widget_flag_for_layout(ui, widget);
 
}

void lf_widget_set_margin(
  lf_ui_state_t* ui,
  lf_widget_t* widget,
  float margin) {
  if(!widget) return;
  if(
    widget->props.margin_top == margin && 
    widget->props.margin_bottom == margin && 
    widget->props.margin_left == margin && 
    widget->props.margin_right == margin) return;

  lf_widget_set_prop(ui, widget, &widget->props.margin_left, margin); 
  lf_widget_set_prop(ui, widget, &widget->props.margin_right, margin); 
  lf_widget_set_prop(ui, widget, &widget->props.margin_top, margin); 
  lf_widget_set_prop(ui, widget, &widget->props.margin_bottom, margin); 

  widget->_changed_size = true;

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
    case LayoutVertical:
      lf_layout_vertical(ui, widget);
      break;
    case LayoutHorizontal:
      lf_layout_horizontal(ui, widget);
      break;
    case LayoutResponsiveGrid:
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
    case LayoutVertical: 
      lf_size_calc_vertical(ui, widget);
      break;
    case LayoutHorizontal:
      lf_size_calc_horizontal(ui, widget);
      break;
    case LayoutResponsiveGrid:
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

void 
lf_widget_set_fixed_width(lf_ui_state_t* ui, lf_widget_t* widget, float width) {
  if(!widget) return;
  if(widget->container.size.x == width) return;
  lf_widget_set_prop(ui, widget, &widget->container.size.x, width);
  widget->_fixed_width = true;
  widget->_changed_size = true;
  lf_widget_shape(ui, lf_widget_flag_for_layout(ui, widget));
}

void 
lf_widget_set_fixed_height(lf_ui_state_t* ui, lf_widget_t* widget, float height) {
  if(!widget) return;
  lf_widget_set_prop(ui, widget, &widget->container.size.y, height);
  widget->_fixed_height = true;
  widget->_changed_size = true;
  lf_widget_shape(ui, lf_widget_flag_for_layout(ui, widget));
}

void 
lf_widget_set_fixed_width_percent(lf_ui_state_t* ui, lf_widget_t* widget, float percent) {
  if(!widget) return;
  if(widget->_width_percent == percent / 100.0f) return;
  widget->_width_percent = percent / 100.0f;
  widget->_fixed_width = true;
  widget->_changed_size = true;
  lf_widget_shape(ui, lf_widget_flag_for_layout(ui, widget));
}

void 
lf_widget_set_fixed_height_percent(lf_ui_state_t* ui, lf_widget_t* widget, float percent) {
  if(!widget) return;
  if(widget->_height_percent == percent / 100.0f) return;
  widget->_height_percent = percent / 100.0f;
  widget->_fixed_height = true;
  widget->_changed_size = true;
  lf_widget_shape(ui, lf_widget_flag_for_layout(ui, widget));
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

void 
lf_widget_apply_size_hints(lf_widget_t* widget) {
  if(widget->_min_size.x != -1.0f) {
    widget->container.size.x = widget->container.size.x < widget->_min_size.x ? widget->_min_size.x : widget->container.size.x; 
  }
  if(widget->_max_size.x != -1.0f) {
    widget->container.size.x = widget->container.size.x > widget->_max_size.x ? widget->_max_size.x : widget->container.size.x; 
  }
  if(widget->_min_size.y != -1.0f) {
    widget->container.size.y = widget->container.size.y < widget->_min_size.y ? widget->_min_size.y : widget->container.size.y; 
  }
  if(widget->_max_size.y != -1.0f) {
    widget->container.size.y = widget->container.size.y > widget->_max_size.y ? widget->_max_size.y : widget->container.size.y; 
  }

  if (widget->_fixed_width && widget->_width_percent != 0.0f && widget->parent) {
    widget->container.size.x = lf_widget_width(widget->parent) * 
      widget->_width_percent - 
      widget->props.padding_left - widget->props.padding_right - 
      widget->parent->props.padding_left - widget->parent->props.padding_right -
    widget->props.margin_left - widget->props.margin_right
    ;
  }
  if (widget->_fixed_height && widget->_height_percent != 0.0f && widget->parent) {
    widget->container.size.y = lf_widget_height(widget->parent) * 
      widget->_height_percent - 
      widget->props.padding_top - widget->props.padding_bottom - 
      widget->parent->props.padding_top -
      widget->props.margin_top - widget->props.margin_bottom
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
  widget->font_style = style;
  if(widget->type == WidgetTypeText) {
    lf_text_set_font(ui, (lf_text_t*)widget, widget->font_family, style, ((lf_text_t*)widget)->font.pixel_size);
  }
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_set_font_style(ui, widget->childs[i], style);
  }
}

void 
lf_widget_set_font_family(lf_ui_state_t* ui, lf_widget_t* widget, const char* font_family) {
  widget->font_family = font_family;
  if(widget->type == WidgetTypeText) {
    lf_text_set_font(ui, (lf_text_t*)widget, font_family, widget->font_style, ((lf_text_t*)widget)->font.pixel_size);
  }
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_set_font_family(ui, widget->childs[i], font_family);
  }
}

void 
lf_widget_set_font_size(lf_ui_state_t* ui, lf_widget_t* widget, uint32_t pixel_size) {
  widget->font_size = pixel_size;
  if(widget->type == WidgetTypeText) {
    lf_text_set_font(ui, (lf_text_t*)widget, widget->font_family, widget->font_style, pixel_size); 
  }
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_set_font_size(ui, widget->childs[i], pixel_size);
  }
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

void lf_widget_set_prop(
  lf_ui_state_t* ui,
  lf_widget_t* widget, 
  float* prop, float val) {
  if(*prop == val) return;
  if(widget->transition_func && ui->delta_time) {
    lf_widget_add_animation(
      widget,
      prop, 
      *prop, val,
      widget->transition_time, widget->transition_func);
  } else {
    *prop = val;
    lf_widget_submit_props(widget);
  }
}


void 
lf_widget_set_prop_color(
  lf_ui_state_t* ui,
  lf_widget_t* widget, 
  lf_color_t* prop, lf_color_t val) {
  lf_widget_set_prop(ui, widget, &prop->r, val.r);
  lf_widget_set_prop(ui, widget, &prop->g, val.g);
  lf_widget_set_prop(ui, widget, &prop->b, val.b);
  lf_widget_set_prop(ui, widget, &prop->a, val.a);
  lf_widget_submit_props(widget);
}

void 
lf_widget_set_visible(lf_widget_t* widget, bool visible) {
  widget->visible = visible;
  widget->parent->_changed_size =true;
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
      to_flag->container.size.y != size_before.y)) && to_flag->parent) {
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
  if(!widget) return;
  widget->container.pos.x = pos + widget->parent->scroll_offset.x;
}

void lf_widget_set_pos_y(lf_widget_t* widget, float pos) {
  if(!widget) return;
  widget->container.pos.y = pos + widget->parent->scroll_offset.y;
}
