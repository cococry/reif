#include "../include/leif/ez_api.h"

static void _end_widget(lf_ui_state_t* ui);
static lf_widget_t* _get_assignment_widget(lf_ui_state_t* ui, lf_widget_type_t type);
static uint32_t _get_font_size(lf_ui_state_t* ui, lf_text_level lvl);
static lf_text_t* _text_create_from_level(lf_ui_state_t* ui, const char* label, lf_text_level lvl); 

static uint32_t font_sizes[] = {
  36, // H1 
  28, // H2
  22, // H3
  18, // H4
  15, // H5
  13, // H6
  16  // Paragraph
};
void _end_widget(lf_ui_state_t* ui) {
  if (!ui->_ez.last_parent || !ui->_ez.last_parent->parent) return;
  ui->_ez.last_parent = ui->_ez.last_parent->parent;
  ui->_ez._assignment_idx = ui->_ez._last_assignment_idx; 
}

lf_widget_t*
_get_assignment_widget(lf_ui_state_t* ui, lf_widget_type_t type) {
  if (!ui->_ez.last_parent || ui->_ez._assignment_idx >= ui->_ez.last_parent->num_childs) {
    fprintf(stderr, "leif: mismatch in widget tree.\n");
    return NULL;
  }

  lf_widget_t* widget = ui->_ez.last_parent->childs[ui->_ez._assignment_idx];

  if (widget->type != type) {
    fprintf(stderr, "leif: mismatch in widget tree. widget ID: %i.\n", widget->id);
    return NULL;
  }

  ui->_ez.current_widget = widget;
  ui->_ez.last_parent = widget;  
  ui->_ez._last_assignment_idx = ui->_ez._assignment_idx + 1;
  ui->_ez._assignment_idx = 0;
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
  if(!ui->_ez._assignment_only) {
    lf_mapped_font_t font = lf_asset_manager_request_font(
      ui, 
      ui->_ez.current_widget->font_family, 
      ui->_ez.current_widget->font_style, 
      _get_font_size(ui, lvl)); 

    lf_text_t* txt = lf_text_create_ex(ui, ui->_ez.last_parent, label, font);
    ui->_ez.current_widget = &txt->base;
    ui->_ez._assignment_idx++;
    return txt;
  } else {
    if(ui->_ez._assignment_idx >= ui->_ez.last_parent->num_childs) return NULL;
    lf_widget_t* widget = ui->_ez.last_parent->childs[ui->_ez._assignment_idx++];
    if(widget->id == 4) {
      printf("Correct!\n");
    }
    if(widget->type != WidgetTypeText) {
      fprintf(stderr,"leif: mismatch in widget tree. widget ID: %i.\n", widget->id);
      return NULL;
    }
    lf_text_set_label(ui, (lf_text_t*)widget, label);
      printf("Setting label!\n");
    ui->_ez.current_widget = widget; 
    return (lf_text_t*)widget;
  }

}

lf_ez_state_t 
lf_ez_api_init(lf_ui_state_t* ui) {
  lf_ez_state_t state = (lf_ez_state_t){
    .current_widget = ui->root, 
    .last_parent = ui->root,
    ._assignment_idx = 0,
    ._last_assignment_idx = 0,
  };
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
  if(!ui->_ez.last_parent) return NULL;
  if(!ui->_ez._assignment_only) {
    lf_div_t* div = lf_div_create(ui, ui->_ez.last_parent);
    ui->_ez.last_parent = &div->base;
    ui->_ez.current_widget = &div->base;
    ui->_ez._last_assignment_idx = ui->_ez._assignment_idx;
    ui->_ez._assignment_idx = 0;
    return div;
  } else {
    return (lf_div_t*)_get_assignment_widget(ui, WidgetTypeDiv);
  }
}

void 
lf_div_end(lf_ui_state_t* ui) {
  _end_widget(ui);
}

lf_button_t*
lf_button(lf_ui_state_t* ui) {
  if(!ui->_ez._assignment_only) {
    lf_button_t* btn = lf_button_create(ui, ui->_ez.last_parent);
    ui->_ez.last_parent = &btn->base;
    ui->_ez.current_widget = &btn->base;
    ui->_ez._last_assignment_idx = ui->_ez._assignment_idx + 1;
    ui->_ez._assignment_idx = 0;
    return btn;
  } else {
    return (lf_button_t*)_get_assignment_widget(ui, WidgetTypeButton);
  }
}

void
lf_button_end(lf_ui_state_t* ui) {
  _end_widget(ui);
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
    ui->_ez.current_widget = &txt->base;
    ui->_ez._assignment_idx++;
    return txt;
  } else {
    if(ui->_ez._assignment_idx >= ui->_ez.last_parent->num_childs) {
      fprintf(stderr,"leif: mismatch in widget tree.\n");
      return NULL;
    } 
    lf_widget_t* widget = ui->_ez.last_parent->childs[ui->_ez._assignment_idx++];
    if(widget->type != WidgetTypeText) {
      fprintf(stderr,"leif: mismatch in widget tree. widget ID: %i.\n", widget->id);
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
    ui->_ez.current_widget = &img->base;
    ui->_ez._assignment_idx++;
    return img; 
  } else {
    if(ui->_ez._assignment_idx >= ui->_ez.last_parent->num_childs) {
      fprintf(stderr,"leif: mismatch in widget tree.\n");
      return NULL;
    }
    lf_widget_t* widget = ui->_ez.last_parent->childs[ui->_ez._assignment_idx++];
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
    ui->_ez.current_widget = &img->base;
    ui->_ez._assignment_idx++;
    return img; 
  } else {
  if(ui->_ez._assignment_idx >= ui->_ez.last_parent->num_childs) {
      fprintf(stderr,"leif: mismatch in widget tree.\n");
      return NULL;
    }
    lf_widget_t* widget = ui->_ez.last_parent->childs[ui->_ez._assignment_idx++];
    if(widget->type != WidgetTypeImage) {
      fprintf(stderr,"leif: mismatch in widget tree. widget ID\n");
      return NULL;
    }
    ui->_ez.current_widget = widget; 



    return (lf_image_t*)widget;
  }
}

lf_image_t* 
lf_image_sized_w(lf_ui_state_t* ui, const char* filepath, uint32_t w) {
  if(!ui->_ez._assignment_only) {
    lf_image_t* img = lf_image_create_ex_w(ui, ui->_ez.last_parent, filepath, w);
    ui->_ez.current_widget = &img->base;
    ui->_ez._assignment_idx++;
  return img; 
  } else {
    if(ui->_ez._assignment_idx >= ui->_ez.last_parent->num_childs) {
      fprintf(stderr,"leif: mismatch in widget tree.\n");
      return NULL;
    }
    lf_widget_t* widget = ui->_ez.last_parent->childs[ui->_ez._assignment_idx++];
    if(widget->type != WidgetTypeImage) {
      fprintf(stderr,"leif: mismatch in widget tree. widget ID\n");
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
    ui->_ez._assignment_idx++;
    return img; 
  } else {
  if(ui->_ez._assignment_idx >= ui->_ez.last_parent->num_childs) {
      fprintf(stderr,"leif: mismatch in widget tree.\n");
      return NULL;
    }
    lf_widget_t* widget = ui->_ez.last_parent->childs[ui->_ez._assignment_idx++];
    if(widget->type != WidgetTypeImage) {
      fprintf(stderr,"leif: mismatch in widget tree. widget ID\n");
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
    ._child_idx = ui->_ez._assignment_idx,
    ._parent = ui->_ez.last_parent
  }));
  lf_div(ui);
  lf_crnt(ui)->sizing_type = SizingFitToContent;
  comp_func();
  lf_div_end(ui);
}

void lf_component_rerender(lf_ui_state_t* ui, lf_component_func_t comp_func) {
  for (uint32_t i = 0; i < ui->_ez.comps.size; i++) {
    lf_component_t* comp = &ui->_ez.comps.items[i];

    if (comp->func == comp_func) {
      lf_ez_api_set_assignment_only_mode(ui, true);

      ui->_ez._assignment_idx = comp->_child_idx;
      ui->_ez.last_parent = comp->_parent;
      if(ui->_ez.last_parent != ui->root || ui->_ez._assignment_idx != 1) {
        printf("Fuck...\n");
      }
      lf_div(ui);
      lf_crnt(ui)->sizing_type = SizingFitToContent;
      comp->func();  // Run component
      lf_div_end(ui);

      lf_ez_api_set_assignment_only_mode(ui, false);
      
      if (comp->_parent && comp->_parent->num_childs > comp->_child_idx) {
        lf_ui_core_rerender_widget(ui, comp->_parent->childs[comp->_child_idx]);
      }

      return;
    }
  }
}
