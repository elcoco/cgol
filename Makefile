CC = gcc
CFLAGS =
BUILD_DIR = build
BIN_DIR = .
SRC_DIR = src

# create build dir
$(shell mkdir -p $(BUILD_DIR))

# target: dependencies
# 	  action

default: node.o main.o
	$(CC) $(BUILD_DIR)/node.o $(BUILD_DIR)/main.o $(CFLAGS) -o $(BIN_DIR)/seagull

main.o: $(SRC_DIR)/main.c 
	$(CC) -c $(SRC_DIR)/main.c -o $(BUILD_DIR)/main.o

node.o: $(SRC_DIR)/node.c $(SRC_DIR)/node.h
	$(CC) -c $(SRC_DIR)/node.c -o $(BUILD_DIR)/node.o

clean:
	rm $(BUILD_DIR)/*.o

install:
	cp -f dface ~/bin/apps
	chmod +x ~/bin/apps/dface
