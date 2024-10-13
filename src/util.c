#include "../include/leif/util.h"
#include "../include/leif/ui_core.h"
#include <runara/runara.h>

bool
lf_point_intersets_container(vec2s point, lf_container_t container) {
    return (point.x >= container.pos.x &&
            point.x <= container.pos.x + container.size.x &&
            point.y >= container.pos.y &&
            point.y <= container.pos.y + container.size.y);
 }

bool 
lf_container_intersets_container(lf_container_t a, lf_container_t b) {
    if (a.pos.x + a.size.x <= b.pos.x ||  
        b.pos.x + b.size.x <= a.pos.x || 
        a.pos.y + a.size.y <= b.pos.y ||
        b.pos.y + b.size.y <= a.pos.y) {  
        return false;
    }
    return true;
}
float 
lf_container_area(lf_container_t container) {
  return container.size.x * container.size.y;
}

lf_font_t 
lf_load_font(lf_ui_state_t* ui, const char* filepath, uint32_t size) {
  return ui->render_font_create(ui->render_state, filepath, size);
}
lf_font_t 
lf_load_font_from_name(lf_ui_state_t* ui, const char* font_name, uint32_t size) {
  return ui->render_font_create(ui->render_state, ui->render_font_file_from_name(font_name), size);
}

void 
lf_font_resize(lf_ui_state_t* ui, lf_font_t font, uint32_t size) {
  rn_set_font_size(ui->render_state, font, size);
}

uint32_t 
lf_font_get_size(lf_ui_state_t* ui, lf_font_t font) {
  return ui->render_font_get_size(font);
}
