#include "../include/leif/asset_manager.h"
#include "../include/leif/ui_core.h"
#include <string.h>

lf_asset_manager_t* 
lf_asset_manager_init(void) {
  lf_asset_manager_t* mgr = (lf_asset_manager_t*)malloc(sizeof(*mgr));
  lf_vector_init(&mgr->textures);

  return mgr;
}

lf_mapped_texture_t 
lf_asset_manager_request_texture(lf_ui_state_t* ui, lf_asset_manager_t* mgr, const char* filepath) {
  for(uint32_t i = 0; i < mgr->textures.size; i++) {
    if(strcmp(mgr->textures.items[i].filepath, filepath) == 0) {
      return mgr->textures.items[i];
    }
  }

  lf_mapped_texture_t tex;
  tex.filepath = strdup(filepath);
  ui->render_load_texture(filepath, &tex.id, &tex.width, &tex.height, 0);
  lf_vector_append(&mgr->textures, tex);

  return tex;
}
