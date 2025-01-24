#include "../include/leif/widget.h"
#include "../include/leif/ui_core.h"
#include "../include/leif/layout.h"
#include "../include/leif/animation.h"
#include "../include/leif/widgets/text.h"
#include <string.h>
#include <time.h>

#ifdef LF_RUNARA
#include <runara/runara.h>
#endif

#define INIT_CHILD_CAP 4

static void widget_resize_children(lf_widget_t* widget, uint32_t new_cap);
static void widget_animate(lf_ui_state_t* ui, lf_widget_t* widget);
static uint32_t count_anims(lf_animation_t* head);

void 
widget_resize_children(lf_widget_t* widget, uint32_t new_cap) {
  widget->childs = (lf_widget_t**)realloc(widget->childs, new_cap * sizeof(lf_widget_t*));
  widget->cap_childs = new_cap;
}

void 
widget_animate(lf_ui_state_t* ui, lf_widget_t* widget) {
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

uint32_t 
count_anims(lf_animation_t* head) {
  int count = 0;
  lf_animation_t* current = head;
  while (current != NULL) {
    count++;
    current = current->next;
  }

  return count;
}
lf_widget_t* 
lf_widget_create(
  uint32_t id,
  lf_widget_type_t type,
  lf_container_t fallback_container,
  lf_widget_props_t props,
  lf_widget_render_cb render,
  lf_widget_handle_event_cb handle_event,
  lf_widget_shape_cb shape) {
  lf_widget_t* widget = (lf_widget_t*)malloc(sizeof(lf_widget_t));

  widget->parent = NULL;
  widget->childs = NULL;
  widget->cap_childs = 0;
  widget->num_childs = 0;
  widget->id = id;
  widget->visible = true;
  widget->_marked_for_removal = false;

  widget->type = type;
  widget->container = fallback_container;
  widget->props = props;
  widget->_initial_props = props;
  widget->_rendered_props = props;
  widget->justify_type = JustifyStart;
  widget->sizing_type = SizingFitToParent;

  widget->render = render;
  widget->handle_event = handle_event;
  widget->shape = shape;

  widget->listening_for = 0;
  widget->alignment_flags = 0;

  widget->_fixed_width = false;
  widget->_fixed_height = false;
  widget->_changed_by_layout = false;

  widget->anims = NULL; 

  widget->_width_percent = 0.0f;
  widget->_height_percent = 0.0f;
  
  widget->_min_size = (vec2s){.x = -1.0f, .y = -1.0f};
  widget->_max_size = (vec2s){.x = -1.0f, .y = -1.0f};

  return widget;
}

lf_widget_t* get_first_intersecting_parent(lf_widget_t* widget) {
  lf_widget_t* current = widget->parent;
  while (current != NULL) {
    if (lf_container_intersets_container(LF_WIDGET_CONTAINER(widget), LF_WIDGET_CONTAINER(current))) {
      return current; 
    }
    current = current->parent; 
  }
  return NULL; 
}

static float widget_get_cull_end_y(lf_widget_t* widget) {
  return 
        widget->container.pos.y + 
        widget->props.padding_top +
        widget->props.padding_bottom +
        widget->container.size.y - 
        widget->props.border_width;
} 

static float widget_get_cull_end_x(lf_widget_t* widget) {
  return 
        widget->container.pos.x + 
        widget->props.padding_left + 
        widget->props.padding_right + 
        widget->container.size.x - 
        widget->props.border_width;
} 

void
lf_widget_render(lf_ui_state_t* ui,  lf_widget_t* widget) {
  if(!widget->visible) return;
  if(widget->render) {
    if(!lf_container_intersets_container(
      LF_WIDGET_CONTAINER(widget), ui->root->container) || !lf_container_intersets_container(LF_WIDGET_CONTAINER(widget), LF_WIDGET_CONTAINER(widget->parent))) {
      return;
    }

    widget->render(ui, widget);
#ifdef LF_RUNARA
    if(widget->type == WidgetTypeDiv) {

      float parent_end_x = widget_get_cull_end_x(widget->parent); 
      float widget_end_x = widget_get_cull_end_x(widget); 
      float end_x = MIN(widget_end_x, parent_end_x);

      float parent_end_y = widget_get_cull_end_y(widget->parent); 
      float widget_end_y = widget_get_cull_end_y(widget); 
      float end_y = MIN(widget_end_y, parent_end_y);

      rn_set_cull_end_x(
        (RnState*)ui->render_state,
        end_x);

      rn_set_cull_end_y(
        (RnState*)ui->render_state, 
        end_y
      );
      rn_set_cull_start_x(ui->render_state,
                          widget->container.pos.x + widget->props.border_width); 
      rn_set_cull_start_y(ui->render_state,
                          widget->container.pos.y + widget->props.border_width); 
    }

#endif
  }    

  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_render(ui, widget->childs[i]);
  }
#ifdef LF_RUNARA
  if(widget->type == WidgetTypeDiv) {
  rn_unset_cull_end_x((RnState*)ui->render_state);
    rn_unset_cull_end_y((RnState*)ui->render_state);
  rn_unset_cull_start_x((RnState*)ui->render_state);
  rn_unset_cull_start_y((RnState*)ui->render_state);
  }
#endif
}

void lf_widget_shape(
  lf_ui_state_t* ui,
  lf_widget_t* widget) {
  if (!widget->shape) return;
  if(widget->parent) {
    if(!lf_container_intersets_container(
      LF_WIDGET_CONTAINER(widget), ui->root->container) || 
      !lf_container_intersets_container(LF_WIDGET_CONTAINER(widget), LF_WIDGET_CONTAINER(widget->parent))) {
      return;
    }
  }
  widget->shape(ui, widget);
  if(widget == ui->root)
    printf("SHaped root!\n");
  for (uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_shape(ui, widget->childs[i]);
  }
}

bool lf_widget_animate(
  lf_ui_state_t* ui,
  lf_widget_t* widget,
  lf_widget_t** o_shape) {  
  bool animated = false;

  uint32_t n_anims = count_anims(widget->anims);
  if (n_anims != 0) {
    widget_animate(ui, widget);
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
  return count_anims(widget->anims) != 0; 
}

void
lf_widget_handle_event(lf_ui_state_t* ui, lf_widget_t* widget, lf_event_t event) {
  if(!widget) return;
  if(widget->handle_event && lf_widget_is_listening(widget, event.type)) {
    widget->handle_event(ui, widget, event); 
  }

  if(!widget) {
    return;
  }
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_handle_event(ui, widget->childs[i], event);
  }
}

int32_t
lf_widget_add_child(lf_widget_t* parent, lf_widget_t* child) {
  if(!parent || !child) return 1;

  if(parent->num_childs >= parent->cap_childs) {
    uint32_t new_cap = parent->cap_childs == 0 ? 2 : parent->cap_childs * 2;
    widget_resize_children(parent, new_cap);
  }
  parent->childs[parent->num_childs++] = child;
  child->parent = parent;
  
  return 0;
}

void
lf_widget_remove(lf_widget_t* widget) {
  if (!widget) return;
  widget->_marked_for_removal = true;

  for (uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_remove(widget->childs[i]);
  }
}

void
lf_widget_remove_from_memory(lf_widget_t* widget) {
  if (!widget) return;

  for (uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_remove(widget->childs[i]);
  }

  if (widget->childs) {
    free(widget->childs);
    widget->childs = NULL;
  }

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
}

void lf_widget_remove_child_from_memory(lf_widget_t* parent, uint32_t child_idx) {
  if (!parent || child_idx >= parent->num_childs) return;

  lf_widget_t* child = parent->childs[child_idx];
  free(child);

  for (uint32_t i = child_idx; i < parent->num_childs - 1; i++) {
    parent->childs[i] = parent->childs[i + 1];
  }
  parent->num_childs--;

  parent->childs = realloc(parent->childs, parent->num_childs * sizeof(lf_widget_t*));
}

float 
lf_widget_width(lf_widget_t* widget) {
  return lf_widget_width_ex(widget, widget->props); 
}

float 
lf_widget_height(lf_widget_t* widget) {
  return lf_widget_height_ex(widget, widget->props); 
}

float 
lf_widget_width_ex(lf_widget_t* widget, lf_widget_props_t props) {
  return widget->container.size.x + 
  props.padding_left      +
  props.padding_right;
}

float 
lf_widget_height_ex(lf_widget_t* widget, lf_widget_props_t props) {
  return widget->container.size.y + 
  props.padding_top       +
  props.padding_bottom;
}

void lf_widget_set_padding(
  lf_widget_t* widget,
  float padding) {
  if(
    widget->props.padding_top == padding && 
    widget->props.padding_bottom == padding && 
    widget->props.padding_left == padding && 
    widget->props.padding_right == padding) return;

  widget->props.padding_top = padding;
  widget->props.padding_bottom = padding;
  widget->props.padding_left = padding; 
  widget->props.padding_right = padding;

  lf_widget_submit_props(widget);
}

void lf_widget_set_margin(
  lf_widget_t* widget,
  float margin) {
  if(
    widget->props.margin_top == margin && 
    widget->props.margin_bottom == margin && 
    widget->props.margin_left == margin && 
    widget->props.margin_right == margin) return;

  widget->props.margin_top = margin;
  widget->props.margin_bottom = margin;
  widget->props.margin_left = margin; 
  widget->props.margin_right = margin;
  lf_widget_submit_props(widget);
}

void lf_widget_set_color(
  lf_ui_state_t* ui,
  lf_widget_t* widget,
  lf_color_t color) {
  if(lf_color_equal(widget->props.color, color)) return;

  widget->props.color = color;
  lf_widget_submit_props(widget);

  ui->root_needs_render = true;
}

void lf_widget_set_border_color(
  lf_ui_state_t* ui,
  lf_widget_t* widget,
  lf_color_t color) {
  if(lf_color_equal(widget->props.border_color, color)) return;

  widget->props.border_color = color;
  lf_widget_submit_props(widget);

  ui->root_needs_render = true;
}

void lf_widget_set_border_width(
  lf_ui_state_t* ui,
  lf_widget_t* widget,
  float border_width) {
  if(widget->props.border_width == border_width) return;

  widget->props.border_width = border_width;
  lf_widget_submit_props(widget);

  ui->root_needs_render = true;
}

void lf_widget_set_corner_radius(
  lf_ui_state_t* ui,
  lf_widget_t* widget,
  float corner_radius) {
  if(widget->props.corner_radius == corner_radius) return;

  widget->props.corner_radius = corner_radius;
  lf_widget_submit_props(widget);

  ui->root_needs_render = true;
}

void 
lf_widget_set_layout(lf_widget_t* widget, lf_layout_type_t layout) {
  widget->layout_type = layout;
}

void 
lf_widget_apply_layout(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(widget->layout_type == LayoutVertical) {
    lf_layout_vertical(ui, widget);
  }
  if(widget->layout_type == LayoutHorizontal) {
    lf_layout_horizontal(ui, widget);
  }
  if(widget->layout_type == LayoutResponsiveGrid) {
    lf_layout_responsive_grid(ui, widget);
  }
}

void 
lf_widget_listen_for(lf_widget_t* widget, uint32_t events) {
  widget->listening_for |= events;
}

void 
lf_widget_unlisten(lf_widget_t* widget, uint32_t events) {
  widget->listening_for &= ~events;
}

bool 
lf_widget_is_listening(lf_widget_t* widget, uint32_t events) {
  return (widget->listening_for & events) != 0;
}

void 
lf_widget_set_listener(lf_widget_t* widget, lf_widget_handle_event_cb cb, uint32_t events) {
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
  lf_animation_interrupt(widget->anims, target);
}

void lf_widget_interrupt_all_animations(
    lf_widget_t* widget) {
  lf_animation_t* anim = widget->anims;
  while(anim) {
    anim->active = false;
    anim = anim->next;
  }
}

void 
lf_widget_submit_props(lf_widget_t* widget) {
  widget->_initial_props = widget->props;
  widget->_rendered_props = widget->props;
}

void 
lf_widget_set_fixed_width(lf_widget_t* widget, float width) {
  widget->container.size.x = width;
  widget->_fixed_width = true;
}

void 
lf_widget_set_fixed_height(lf_widget_t* widget, float height) {
  widget->container.size.y = height;
  widget->_fixed_height = true;
}

void 
lf_widget_set_fixed_width_percent(lf_widget_t* widget, float percent) {
  widget->_width_percent = percent / 100.0f;
  widget->_fixed_width = true;
}

void 
lf_widget_set_fixed_height_percent(lf_widget_t* widget, float percent) {
  widget->_height_percent = percent / 100.0f;
  widget->_fixed_height = true;
}

void lf_widget_set_alignment(lf_widget_t* widget, uint32_t flags) {
  lf_flag_set(&widget->alignment_flags, flags);
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
