CC = gcc
CFLAGS =
BUILD_DIR = build
BIN_DIR = .
SRC_DIR = src

# create build dir
$(shell mkdir -p $(BUILD_DIR))

# target: dependencies
# 	  action

default: utils.o node.o seed.o main.o
	$(CC) $(BUILD_DIR)/utils.o $(BUILD_DIR)/node.o $(BUILD_DIR)/seed.o $(BUILD_DIR)/main.o $(CFLAGS) -o $(BIN_DIR)/seagull

utils.o: $(SRC_DIR)/utils.c $(SRC_DIR)/utils.h
	$(CC) -c $(SRC_DIR)/utils.c -o $(BUILD_DIR)/utils.o

main.o: $(SRC_DIR)/main.c 
	$(CC) -c $(SRC_DIR)/main.c -o $(BUILD_DIR)/main.o

node.o: $(SRC_DIR)/node.c $(SRC_DIR)/node.h
	$(CC) -c $(SRC_DIR)/node.c -o $(BUILD_DIR)/node.o

seed.o: $(SRC_DIR)/seed.c $(SRC_DIR)/seed.h
	$(CC) -c $(SRC_DIR)/seed.c -o $(BUILD_DIR)/seed.o

clean:
	rm $(BUILD_DIR)/*.o

install:
	cp -f dface ~/bin/apps
	chmod +x ~/bin/apps/dface
