WINDOWING=-DLF_X11
PLATFORM_SPECIFIC = src/platform/win_glfw.c  src/platform/win_x11.c  
PLATFORM_SPECIFIC_LIBS = -lglfw -lX11 -lXrandr -lXrender
ADDITIONAL_FLAGS=-DLF_RUNARA
RUNARA_LIBS=-lrunara -lharfbuzz -lfreetype -lm -lGL
