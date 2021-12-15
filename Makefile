CC = gcc
CFLAGS = -g -pthread -Wall
BUILD_DIR = build
BIN_DIR = .
SRC_DIR = src

# create build dir
$(shell mkdir -p $(BUILD_DIR))

# target: dependencies
# 	  action

SRC = $(shell find src -name '*.c')
OBJ = ${SRC:.c=.o}

seagull: ${OBJ}
	${CC} -o $@ ${OBJ} ${CFLAGS}

clean:
	rm $(BUILD_DIR)/*.o

install:
	cp -f dface ~/bin/apps
	chmod +x ~/bin/apps/dface
