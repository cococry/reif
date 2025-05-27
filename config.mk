WINDOWING=-DLF_X11
PLATFORM_SPECIFIC = src/platform/win_glfw.c  src/platform/win_x11.c  
PLATFORM_SPECIFIC_LIBS = -lglfw
ADDITIONAL_FLAGS=-DLF_RUNARA
RUNARA_LIBS=-lrunara -lharfbuzz -lfreetype -lm -lGL
