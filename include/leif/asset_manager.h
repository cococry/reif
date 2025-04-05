#pragma once
#include "render.h"
#include "util.h"
#include "win.h"

#include <fontconfig/fontconfig.h>
#include <freetype2/ft2build.h>
#include <harfbuzz/hb.h>

typedef struct {
  lf_texture_id id;
  uint32_t width, height;
  const char* filepath;
} lf_mapped_texture_t;

typedef struct {
  char* filepath;
  lf_font_style_t style;
  uint32_t face_idx;
} lf_loaded_font_style_t; 

typedef struct {
  lf_loaded_font_style_t styles[LF_FONT_STYLE_COUNT];
  uint32_t nstyles;
  char* family_name;
} lf_font_style_list_t;

typedef struct {
  uint32_t pixel_size;
  lf_loaded_font_style_t style;
  char* family_name;
  lf_font_t font;

  lf_window_t _render_win;
  FT_Face _ft_face;
  hb_font_t* _hb_font;
  float _space_w;
} lf_mapped_font_t;

typedef struct {
  lf_mapped_texture_t* items;
  uint32_t cap, size;
} lf_mapped_texture_array_t;

typedef struct {
  lf_mapped_font_t* items;
  uint32_t cap, size;
} lf_mapped_font_array_t;

typedef struct {
  lf_mapped_texture_array_t textures;
  lf_mapped_font_array_t fonts;
  
  FcConfig* fc_config;
  FcObjectSet* fc_os;
} lf_asset_manager_t;

lf_asset_manager_t* lf_asset_manager_init(void);

void lf_asset_manager_terminate(lf_asset_manager_t* mgr);

lf_mapped_texture_t lf_asset_manager_request_texture(lf_ui_state_t* ui, const char* filepath);

lf_mapped_font_t lf_asset_manager_request_font(lf_ui_state_t* ui, const char* family_name, lf_font_style_t style, uint32_t pixel_size); 

lf_font_style_list_t lf_asset_manager_get_font_styles_from_family(lf_ui_state_t* ui, const char* family_name); 

lf_loaded_font_style_t lf_asset_manager_get_loaded_font_style(
    lf_ui_state_t* ui, 
    const char* family_name, 
    lf_font_style_t style); 
