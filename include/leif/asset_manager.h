#pragma once
#include "render.h"

typedef struct {
  lf_texture_id id;
  uint32_t width, height;
  const char* filepath;
} lf_mapped_texture_t;

typedef struct {
  lf_mapped_texture_t* items;
  uint32_t cap, size;
} lf_mapped_texture_array_t;


typedef struct {
  lf_mapped_texture_array_t textures;
} lf_asset_manager_t;

lf_asset_manager_t* lf_asset_manager_init(void);

lf_mapped_texture_t lf_asset_manager_request_texture(lf_ui_state_t* ui, lf_asset_manager_t* mgr, const char* filepath);
