WINDOWING=-DLF_GLFW
PLATFORM_SPECIFIC = src/platform/win_glfw.c 
PLATFORM_SPECIFIC_LIBS = -lglfw
ADDITIONAL_FLAGS=-DLF_RUNARA
RUNARA_LIBS=-lrunara -lharfbuzz -lfreetype -lm -lGL
