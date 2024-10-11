CC			= gcc
BIN			= test
DEPLIBS = -lrunara -lfreetype -lharfbuzz -lfontconfig -lm -lGL -lglfw
CFLAGS	= -DLF_GLFW -DLF_RUNARA -ffast-math -O3 -pedantic -Wall -Werror -Wextra
LIBS 		=-L../lib -lleif  ${DEPLIBS} 

all:
	${CC} -o bin/${BIN}  ${CFLAGS}  *.c ${LIBS}
