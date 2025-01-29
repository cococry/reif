#include "../include/leif/asset_manager.h"
#include "../include/leif/ui_core.h"
#include <fontconfig/fontconfig.h>
#include <stdio.h>
#include <string.h>

static const char* font_style_mappings[LF_FONT_STYLE_COUNT] = {
  "Regular",
  "Italic",
  "Oblique",
  "Bold",
  "Bold Italic",
  "SemiBold",
  "SemiBold Italic",
  "Light",
  "Light Italic",
  "ExtraLight",
  "ExtraLight Italic",
  "Medium",
  "Medium Italic",
  "Thin",
  "Thin Italic",
  "ExtraBold",
  "ExtraBold Italic",
  "Condensed",
  "Condensed Italic",
  "Expanded",
  "Expanded Italic",
};

lf_asset_manager_t* 
lf_asset_manager_init(void) {
  lf_asset_manager_t* mgr = (lf_asset_manager_t*)malloc(sizeof(*mgr));
  lf_vector_init(&mgr->textures);
  lf_vector_init(&mgr->fonts);

  mgr->fc_config  = NULL; 
  mgr->fc_os      = NULL; 

  return mgr;
}

void 
lf_asset_manager_terminate(lf_asset_manager_t* mgr) {
  if(mgr->fonts.size)
    lf_vector_free(&mgr->fonts);
  if(mgr->textures.size)
  lf_vector_free(&mgr->textures);

  if(mgr->fc_config)
    FcConfigDestroy(mgr->fc_config);
  if(mgr->fc_os)
    FcObjectSetDestroy(mgr->fc_os);
}

lf_mapped_texture_t 
lf_asset_manager_request_texture(lf_ui_state_t* ui, const char* filepath) {
  if(!ui || !ui->asset_manager) return (lf_mapped_texture_t){0};
  for(uint32_t i = 0; i < ui->asset_manager->textures.size; i++) {
    if(strcmp(ui->asset_manager->textures.items[i].filepath, filepath) == 0) {
      return ui->asset_manager->textures.items[i];
    }
  }

  lf_mapped_texture_t tex;
  tex.filepath = strdup(filepath);
  ui->render_load_texture(filepath, &tex.id, &tex.width, &tex.height, 0);
  lf_vector_append(&ui->asset_manager->textures, tex);

  return tex;
}

lf_font_style_list_t 
lf_asset_manager_get_font_styles_from_family(lf_ui_state_t* ui, const char* family_name) {
  if(!ui || !ui->asset_manager->fonts.items) return (lf_font_style_list_t){0}; 
  if(!ui->asset_manager->fc_config) {
    ui->asset_manager->fc_config = FcInitLoadConfigAndFonts();
  }

  FcPattern* pat = FcPatternCreate();
  FcPatternAddString(pat, FC_FAMILY, (const FcChar8*)family_name);

  if(!ui->asset_manager->fc_os)
    ui->asset_manager->fc_os =  FcObjectSetBuild(
    FC_FAMILY, FC_STYLE, FC_LANG, FC_FILE, FC_INDEX, 
    (char *)0);

  FcFontSet* fs = FcFontList(
    ui->asset_manager->fc_config,
    pat, ui->asset_manager->fc_os);

  lf_font_style_list_t fl;
  fl.family_name = strdup(family_name);
  fl.nstyles = 0;
  memset(fl.styles, 0, sizeof(fl.styles));

  if (fs) {
    for (int i = 0; i < fs->nfont; ++i) {
      FcPattern* font = fs->fonts[i];
      FcChar8 *file, *style;
      int index = -1;
      FcPatternGetInteger(font, FC_INDEX, 0, &index);
      if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch &&
        FcPatternGetString(font, FC_STYLE, 0, &style) == FcResultMatch) { 
        lf_font_style_t style_i = 0; 
        for(uint32_t j = 0; j < LF_FONT_STYLE_COUNT - 1; j++) {
          if(strcmp((char*)style, font_style_mappings[j]) == 0) {
            style_i = (lf_font_style_t)j;
            break;
          } 
        }
        fl.styles[fl.nstyles].style = style_i;
        fl.styles[fl.nstyles].filepath = NULL;
        fl.styles[fl.nstyles].face_idx = index;

        char* file_copy =  strdup((const char*)file);
        if(file_copy == NULL) {
          printf("leif: failed to allocate memory for font filepath.\n");
          continue;
        }
        fl.styles[fl.nstyles].filepath = file_copy;
        fl.nstyles++;
      }
    }
    FcFontSetDestroy(fs);
  }

  if (pat) FcPatternDestroy(pat);

  return fl;
}

lf_loaded_font_style_t lf_asset_manager_get_loaded_font_style(
    lf_ui_state_t* ui, 
    const char* family_name, 
    lf_font_style_t style) {
  if(!ui) return (lf_loaded_font_style_t){0}; 
  if(!ui->asset_manager->fc_config) {
    ui->asset_manager->fc_config = FcInitLoadConfigAndFonts();
  }

  FcPattern* pat = FcPatternCreate();
  FcPatternAddString(pat, FC_FAMILY, (const FcChar8*)family_name);

  if(!ui->asset_manager->fc_os)
    ui->asset_manager->fc_os =  FcObjectSetBuild(
    FC_FAMILY, FC_STYLE, FC_LANG, FC_FILE, FC_INDEX, 
    (char *)0);

  FcFontSet* fs = FcFontList(
    ui->asset_manager->fc_config,
    pat, ui->asset_manager->fc_os);

  if(fs) {
    for (int i = 0; i < fs->nfont; ++i) {
      FcPattern* font = fs->fonts[i];
      FcChar8 *file_str, *style_str;
      int index = -1;
      FcPatternGetString(font, FC_STYLE, 0, &style_str);
      if(strcmp((char*)style_str, LF_FONT_STYLE_TO_STRING(style)) == 0) {
        if( FcPatternGetInteger(font, FC_INDEX, 0, &index) != FcResultMatch || 
            FcPatternGetString(font, FC_FILE, 0, &file_str) != FcResultMatch)
          return (lf_loaded_font_style_t){0}; 

        lf_loaded_font_style_t loaded_style;
        loaded_style.style = style;
        loaded_style.filepath = NULL;
        loaded_style.face_idx = index;

        char* file_copy =  strdup((const char*)file_str);
        if(file_copy == NULL) {
          fprintf(stderr, "leif: failed to allocate memory for font filepath.\n");
          continue;
        }
        loaded_style.filepath = file_copy;
        if (pat) FcPatternDestroy(pat);
        FcFontSetDestroy(fs);
        return loaded_style;
      }
    }
  }

  if (pat) FcPatternDestroy(pat);
  FcFontSetDestroy(fs);

  fprintf(stderr, "leif: failed to retrieve font style '%s' from family '%s'.\n",
          LF_FONT_STYLE_TO_STRING(style), family_name);
  return (lf_loaded_font_style_t){0};

}

lf_mapped_font_t
lf_asset_manager_request_font(
  lf_ui_state_t* ui, 
  const char* family_name,
  lf_font_style_t style,
  uint32_t pixel_size) {
  if(!ui || !ui->asset_manager) return (lf_mapped_font_t){0}; 

  lf_mapped_font_t font;
  memset(&font, 0, sizeof(font));

  if(!ui->asset_manager->fonts.items) return font;

  for(uint32_t i = 0; i < ui->asset_manager->fonts.size; i++) {
  lf_mapped_font_t font = ui->asset_manager->fonts.items[i];
    lf_loaded_font_style_t font_style = font.style;
  if(!font_style.filepath) continue;
    if(
      font.pixel_size == pixel_size               && 
      strcmp(font.family_name, family_name) == 0  &&
      font_style.style == style) {
      return font; 
    }
  }

  font.pixel_size = pixel_size;
  font.style = lf_asset_manager_get_loaded_font_style(
    ui, 
    family_name, 
    style);

  font.font = ui->render_font_create_from_face(
    ui->render_state,
    font.style.filepath,
    pixel_size,
    font.style.face_idx
  );
  font.family_name = strdup(family_name);

  lf_vector_append(&ui->asset_manager->fonts, font);

  return font;
}
