#include "../include/leif/ez_api.h"

static int32_t _assign_idx(lf_ui_state_t* ui);
static void _end_widget(lf_ui_state_t* ui);
static lf_widget_t* _get_assignment_widget(lf_ui_state_t* ui, lf_widget_type_t type);
static uint32_t _get_font_size(lf_ui_state_t* ui, lf_text_level lvl);
static lf_text_t* _text_create_from_level(lf_ui_state_t* ui, const char* label, lf_text_level lvl); 
static void _level_deeper(lf_ui_state_t* ui);

static uint32_t font_sizes[] = {
  36, // H1 
  28, // H2
  22, // H3
  18, // H4
  15, // H5
  13, // H6
  16  // Paragraph
};

int32_t
_assign_idx(lf_ui_state_t* ui) {
  return ui->_ez.index_stack[ui->_ez.index_depth];
}

void _end_widget(lf_ui_state_t* ui) {
  if (!ui->_ez.last_parent || !ui->_ez.last_parent->parent) return;

  ui->_ez.last_parent = ui->_ez.last_parent->parent;
  if (ui->_ez.index_depth > 0) {
    ui->_ez.index_depth--;
  }
  ui->_ez.index_stack[ui->_ez.index_depth]++;  
}

lf_widget_t*
_get_assignment_widget(lf_ui_state_t* ui, lf_widget_type_t type) {
  if (!ui->_ez.last_parent || _assign_idx(ui) >= ui->_ez.last_parent->num_childs) {
    fprintf(stderr, "leif: _get_assignment_widget: mismatch in widget tree.\n");
    if(!ui->_ez.last_parent)
      fprintf(stderr, "leif: _get_assignment_widget: assigment parent not valid.\n");
    if(_assign_idx(ui) >= ui->_ez.last_parent->num_childs)
      fprintf(stderr, "leif: _get_assignment_widget: assigment child index not valid (assigment index: %i).\n",
          _assign_idx(ui));
    return NULL;
  }

  lf_widget_t* widget = ui->_ez.last_parent->childs[_assign_idx(ui)];
  widget->_rendered_within_comp = true;

  if (widget->type != type) {
    fprintf(stderr, "leif: _get_assignment_widget: mismatch in widget tree. widget ID: %i.\n", widget->id);
    return NULL;
  }

  ui->_ez.current_widget = widget;
  ui->_ez.last_parent = widget;  
  _level_deeper(ui);
  return widget;
}

uint32_t 
_get_font_size(lf_ui_state_t* ui, lf_text_level lvl) {
  return (ui->_ez.last_parent->font_size != -1 || 
      ui->_ez.current_widget->font_size != -1)   ? 
    (ui->_ez.current_widget->font_size != -1   ? 
     ui->_ez.current_widget->font_size          : 
     ui->_ez.last_parent->font_size)            :
    font_sizes[lvl];
}

lf_text_t* 
_text_create_from_level(lf_ui_state_t* ui, const char* label, lf_text_level lvl) {
  bool overflowing = _assign_idx(ui) >= ui->_ez.last_parent->num_childs;
  if (!ui->_ez._assignment_only || overflowing) {
    lf_mapped_font_t font = lf_asset_manager_request_font(
        ui, 
        ui->_ez.current_widget->font_family, 
        ui->_ez.current_widget->font_style, 
        _get_font_size(ui, lvl)
        ); 
    lf_text_t* txt = lf_text_create_ex(ui, ui->_ez.last_parent, label, font);
    ui->_ez.current_widget = &txt->base;

    ui->_ez.index_stack[ui->_ez.index_depth]++;

    if(overflowing && ui->delta_time) {
      ui->needs_render = true;
      txt->base._changed_size = true;
      lf_widget_shape(ui, lf_widget_flag_for_layout(ui, &txt->base));
    }

    txt->base._rendered_within_comp = true;

    return txt;
  } else {
    lf_widget_t* widget = 
      ui->_ez.last_parent->childs[
      ui->_ez.index_stack[ui->_ez.index_depth]++];

    widget->_rendered_within_comp = true;

    if (widget->type != WidgetTypeText) {
      fprintf(stderr, "leif: _text_create_from_level: mismatch in widget tree. widget ID: %i.\n", widget->id);
      return NULL;
    }

    lf_text_set_label(ui, (lf_text_t*)widget, label);
    ui->_ez.current_widget = widget; 
    return (lf_text_t*)widget;
  }
}

void 
_level_deeper(lf_ui_state_t* ui) {
  ui->_ez.index_depth++;
  ui->_ez.index_stack[ui->_ez.index_depth] = 0;
}

lf_ez_state_t 
lf_ez_api_init(lf_ui_state_t* ui) {
  lf_ez_state_t state = (lf_ez_state_t){
    .current_widget = ui->root, 
      .last_parent = ui->root,
      .index_depth = 0,
  };
  memset(state.index_stack, 0, sizeof(state.index_stack));
  lf_vector_init(&state.comps);
  return state;
}

void
lf_ez_api_terminate(lf_ui_state_t* ui) {
  lf_vector_free(&ui->_ez.comps);
}

void 
lf_ez_api_set_assignment_only_mode(lf_ui_state_t* ui, bool assignment_only) {
  ui->_ez._assignment_only = assignment_only;
}

lf_div_t*
lf_div(lf_ui_state_t* ui) {
  if (!ui->_ez.last_parent) return NULL;
  bool overflowing = _assign_idx(ui) >= ui->_ez.last_parent->num_childs;

  if (!ui->_ez._assignment_only || overflowing) {
    lf_div_t* div = lf_div_create(ui, ui->_ez.last_parent);
    ui->_ez.last_parent = &div->base;
    ui->_ez.current_widget = &div->base;

    _level_deeper(ui);

    if(overflowing && ui->delta_time) {
      ui->needs_render = true;
      div->base._changed_size = true;
      lf_widget_shape(ui, lf_widget_flag_for_layout(ui, &div->base));
    }
    div->base._rendered_within_comp = true;

    return div;
  } else {
    return (lf_div_t*)_get_assignment_widget(ui, WidgetTypeDiv);
  }
}

lf_button_t*
lf_button(lf_ui_state_t* ui) {
  if (!ui->_ez.last_parent) return NULL;
  bool overflowing = _assign_idx(ui) >= ui->_ez.last_parent->num_childs;
  if (!ui->_ez._assignment_only || overflowing) {
    lf_button_t* btn = lf_button_create(ui, ui->_ez.last_parent);

    ui->_ez.last_parent = &btn->base;
    ui->_ez.current_widget = &btn->base;

    _level_deeper(ui);

    if(overflowing && ui->delta_time) {
      ui->needs_render = true;
      btn->base._changed_size = true;
      lf_widget_flag_for_layout(ui, &btn->base);
    }
    btn->base._rendered_within_comp = true;

    return btn;
  } else {
    return (lf_button_t*)_get_assignment_widget(ui, WidgetTypeButton);
  }
}


void 
lf_div_end(lf_ui_state_t* ui) {
  _end_widget(ui);
}
 void
lf_button_end(lf_ui_state_t* ui) {
  _end_widget(ui);
}

lf_slider_t* 
lf_slider(lf_ui_state_t* ui, float* val, float min, float max) {
  bool overflowing = _assign_idx(ui) >= ui->_ez.last_parent->num_childs;
  if (!ui->_ez._assignment_only || overflowing) {
    lf_slider_t* slider = lf_slider_create(ui, ui->_ez.last_parent, val, min, max);
    ui->_ez.current_widget = &slider->base;

    ui->_ez.index_stack[ui->_ez.index_depth]++;

    if(overflowing && ui->delta_time) {
      ui->needs_render = true;
      slider->base._changed_size = true;
      lf_widget_shape(ui, lf_widget_flag_for_layout(ui, &slider->base));
    }

    slider->base._rendered_within_comp = true;

    return slider;
  } else {
    lf_widget_t* widget = 
      ui->_ez.last_parent->childs[
      ui->_ez.index_stack[ui->_ez.index_depth]++];

    widget->_rendered_within_comp = true;

    if (widget->type != WidgetTypeSlider) {
      fprintf(stderr, "leif: lf_slider: mismatch in widget tree. widget ID: %i.\n", widget->id);
      return NULL;
    }

    ((lf_slider_t*)widget)->val = val;
    ((lf_slider_t*)widget)->min = min;
    ((lf_slider_t*)widget)->max = max;
    ui->_ez.current_widget = widget; 
    return (lf_slider_t*)widget;
  }
}

lf_text_t* 
lf_text_p(lf_ui_state_t* ui, const char* label) {
  return _text_create_from_level(ui, label, TextLevelParagraph);
}

lf_text_t* 
lf_text_h1(lf_ui_state_t* ui, const char* label) {
  return _text_create_from_level(ui, label, TextLevelH1);
}

lf_text_t* 
lf_text_h2(lf_ui_state_t* ui, const char* label) {
  return _text_create_from_level(ui, label, TextLevelH2);
}

lf_text_t*
lf_text_h3(lf_ui_state_t* ui, const char* label) {
  return _text_create_from_level(ui, label, TextLevelH3);
}

lf_text_t*
lf_text_h4(lf_ui_state_t* ui, const char* label) {
  return _text_create_from_level(ui, label, TextLevelH4);
}

lf_text_t*
lf_text_h5(lf_ui_state_t* ui, const char* label) {
  return _text_create_from_level(ui, label, TextLevelH5);
}

lf_text_t* 
lf_text_h6(lf_ui_state_t* ui, const char* label) {
  return _text_create_from_level(ui, label, TextLevelH6);
}

lf_text_t* 
lf_text_sized(lf_ui_state_t* ui, const char* label, uint32_t pixel_size) {
  if(!ui->_ez._assignment_only) {
    lf_mapped_font_t font = lf_asset_manager_request_font(
        ui, 
        ui->_ez.current_widget->font_family, 
        ui->_ez.current_widget->font_style, 
        pixel_size);

    lf_text_t* txt = lf_text_create_ex(ui, ui->_ez.last_parent, label, font); 
    ui->_ez.index_stack[ui->_ez.index_depth]++;
    txt->base._rendered_within_comp = true;
    ui->_ez.current_widget = &txt->base;
    return txt;
  } else {
    if(_assign_idx(ui) >= ui->_ez.last_parent->num_childs) {
      fprintf(stderr,"leif: lf_text_sized: mismatch in widget tree.\n");
      return NULL;
    } 
    lf_widget_t* widget = ui->_ez.last_parent->childs[ui->_ez.index_stack[ui->_ez.index_depth]++];
    widget->_rendered_within_comp = true;
    if(widget->type != WidgetTypeText) {
      fprintf(stderr,"leif: lf_text_sized: mismatch in widget tree. widget ID: %i.\n", widget->id);
      return NULL;
    }
    lf_text_set_label(ui, (lf_text_t*)widget, label);
    ui->_ez.current_widget = widget; 
    return (lf_text_t*)widget;
  }
}

lf_text_dimension_t 
lf_text_measure(lf_ui_state_t* ui, const char* text, lf_font_t font) {
  return ui->render_get_text_dimension(ui->render_state, text, font);
}

lf_widget_t*
lf_crnt(lf_ui_state_t* ui) {
  return ui->_ez.current_widget;
}

lf_image_t* 
lf_image(lf_ui_state_t* ui, const char* filepath) {
  if(!ui->_ez._assignment_only) {
    lf_image_t* img = lf_image_create(ui, ui->_ez.last_parent, filepath);
    img->base._rendered_within_comp = true;
    ui->_ez.current_widget = &img->base;
    ui->_ez.index_stack[ui->_ez.index_depth]++;
    printf("created image.\n");
    return img; 
  } else {
    if(_assign_idx(ui) >= ui->_ez.last_parent->num_childs) {
      fprintf(stderr,"leif: mismatch in widget tree.\n");
      return NULL;
    }
    lf_widget_t* widget = ui->_ez.last_parent->childs[
      ui->_ez.index_stack[ui->_ez.index_depth]++];
    widget->_rendered_within_comp = true;
    if(widget->type != WidgetTypeImage) {
      fprintf(stderr,"leif: mismatch in widget tree. widget ID\n");
      return NULL;
    }
    ui->_ez.current_widget = widget; 
    return (lf_image_t*)widget;
  }
}

lf_image_t* 
lf_image_sized(lf_ui_state_t* ui, const char* filepath, uint32_t w, uint32_t h) {
  if(!ui->_ez._assignment_only) {
    lf_image_t* img = lf_image_create_ex(ui, ui->_ez.last_parent, filepath, w, h);
    img->base._rendered_within_comp = true;
    ui->_ez.current_widget = &img->base;
    ui->_ez.index_stack[ui->_ez.index_depth]++;

    return img; 
  } else {
    if(_assign_idx(ui) >= ui->_ez.last_parent->num_childs) {
      fprintf(stderr,"leif: lf_image_sized: mismatch in widget tree.\n");
      return NULL;
    }
    lf_widget_t* widget = ui->_ez.last_parent->childs[ui->_ez.index_stack[ui->_ez.index_depth]++];
    widget->_rendered_within_comp = true;
    if(widget->type != WidgetTypeImage) {
      fprintf(stderr,"leif: lf_image_sized: mismatch in widget tree. widget ID\n");
      return NULL;
    }
    ui->_ez.current_widget = widget; 



    return (lf_image_t*)widget;
  }
}

lf_image_t* 
lf_image_sized_w(lf_ui_state_t* ui, const char* filepath, uint32_t w) {
  bool overflowing = _assign_idx(ui) >= ui->_ez.last_parent->num_childs;
  if(!ui->_ez._assignment_only || overflowing) {
    lf_image_t* img = lf_image_create_ex_w(ui, ui->_ez.last_parent, filepath, w);
    ui->_ez.current_widget = &img->base;
    ui->_ez.index_stack[ui->_ez.index_depth]++;

    img->base._rendered_within_comp = true;
    return img; 
  } else {
    lf_widget_t* widget = ui->_ez.last_parent->childs[ui->_ez.index_stack[ui->_ez.index_depth]++];
    widget->_rendered_within_comp = true;
    if(widget->type != WidgetTypeImage) {
      fprintf(stderr,"leif: lf_image_sized_w: mismatch in widget tree. widget ID\n");
      return NULL;
    }
    ui->_ez.current_widget = widget; 


    return (lf_image_t*)widget;
  }
}


lf_image_t* 
lf_image_sized_h(lf_ui_state_t* ui, const char* filepath, uint32_t h) {
  if(!ui->_ez._assignment_only) {
    lf_image_t* img = lf_image_create_ex_h(ui, ui->_ez.last_parent, filepath, h);
    ui->_ez.current_widget = &img->base;
    ui->_ez.index_stack[ui->_ez.index_depth]++;
    return img; 
  } else {
    if(_assign_idx(ui) >= ui->_ez.last_parent->num_childs) {
      fprintf(stderr,"leif: lf_image_sized_h: mismatch in widget tree.\n");
      return NULL;
    }
    lf_widget_t* widget = ui->_ez.last_parent->childs[
      ui->_ez.index_stack[ui->_ez.index_depth]++
    ];
    if(widget->type != WidgetTypeImage) {
      fprintf(stderr,"leif: lf_image_sized_h: mismatch in widget tree. widget ID\n");
      return NULL;
    }
    ui->_ez.current_widget = widget; 

    return (lf_image_t*)widget;
  }
}

void 
lf_component(lf_ui_state_t* ui, lf_component_func_t comp_func) {
  lf_vector_append(&ui->_ez.comps, ((lf_component_t){
        .func = comp_func, 
        ._child_idx = _assign_idx(ui), 
        ._parent = ui->_ez.last_parent
        }));
  comp_func();
}

void reset_widgets(lf_ui_state_t* ui, lf_widget_t* widget) {
  widget->_rendered_within_comp = false;
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    reset_widgets(ui, widget->childs[i]);
  }
}


bool remove_widgets(lf_ui_state_t* ui, lf_widget_t* widget, lf_widget_t* end, lf_widget_t* comp_widget) {
  if (!widget) return false;  

  bool removed = false;
  if (!widget->_rendered_within_comp) {
    comp_widget->_changed_size = true;
    ui->needs_render = true;
    removed = true;

    if(widget->transition_func) {
      lf_widget_add_animation(widget, &widget->container.size.x, 
                              widget->container.size.x, 0, widget->transition_time, 
                              widget->transition_func);
      lf_widget_add_animation(widget, &widget->container.size.y, 
                              widget->container.size.y, 0, widget->transition_time, 
                              widget->transition_func);
    } 
      lf_widget_remove(widget);
  }

  for (int32_t i = (int32_t)widget->num_childs - 1; i >= 0; i--) {
    removed |= remove_widgets(ui, widget->childs[i], end, comp_widget);
  }

  if (widget == end) return removed;

  return removed;
}

void lf_component_rerender(lf_ui_state_t* ui, lf_component_func_t comp_func) {
  for (uint32_t i = 0; i < ui->_ez.comps.size; i++) {
    lf_component_t* comp = &ui->_ez.comps.items[i];

    if (comp->func == comp_func) {
      lf_ez_api_set_assignment_only_mode(ui, true);

      lf_widget_t* comp_widget = comp->_parent->childs[comp->_child_idx];
      bool needs_reshape = false;
      ui->_ez.index_stack[ui->_ez.index_depth] = comp->_child_idx;
      ui->_ez.last_parent = comp->_parent;
      reset_widgets(ui, comp_widget);
      comp->func();  // Run component
      needs_reshape = remove_widgets(
          ui, comp_widget, 
          ui->_ez.current_widget,
          comp_widget);

      lf_ez_api_set_assignment_only_mode(ui, false);

      if (needs_reshape) {
        lf_widget_flag_for_layout(ui, comp_widget);
      ui->needs_render = true;
      }

      return;
    }
  }
}

void 
lf_skip_widget(lf_ui_state_t* ui) {
  ui->_ez.index_stack[ui->_ez.index_depth]++;
}
