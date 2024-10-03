include config.mk

CC 				= cc
LIBS 			= -lrunara -lfreetype -lharfbuzz -lfontconfig -lm -lGL -lX11 -lleif 
CFLAGS		= ${WINDOWING} ${ADDITIONAL_FLAGS} -Wall -Wextra -Werror -pedantic -O3 -ffast-math 
SRC_FILES = $(wildcard src/*.c src/platform/*.c src/widgets/*.c)
OBJ_FILES = $(SRC_FILES:.c=.o)


all: lib/libleif.a

lib/libleif.a: $(OBJ_FILES)
	mkdir -p lib
	ar cr lib/libleif.a $(OBJ_FILES)
	rm -f $(OBJ_FILES)

%.o: %.c
	${CC} -c $< -o $@ ${LIBS} ${CFLAGS}

lib:
	mkdir -p lib

clean:
	rm -rf lib

install:
	cp lib/libleif.a /usr/local/lib/ 
	cp -r include/leif /usr/local/include/ 

uninstall:
	rm -f /usr/local/lib/libleif.a
	rm -rf /usr/local/include/leif/

.PHONY: all clean install uninstall
