CC = gcc

# lncursesw is used for UTF8 support
CFLAGS = -g -Wall -lncursesw 

BUILD_DIR = build
BIN_DIR = .
SRC_DIR = src

# create build dir
$(shell mkdir -p $(BUILD_DIR))

# target: dependencies
# 	  action

default: ui.o utils.o matrix.o seed.o main.o
	$(CC) $(BUILD_DIR)/ui.o $(BUILD_DIR)/utils.o $(BUILD_DIR)/matrix.o $(BUILD_DIR)/seed.o $(BUILD_DIR)/main.o $(CFLAGS) -o $(BIN_DIR)/seagull

utils.o: $(SRC_DIR)/utils.c $(SRC_DIR)/utils.h
	$(CC) -c $(SRC_DIR)/utils.c -o $(BUILD_DIR)/utils.o

main.o: $(SRC_DIR)/main.c 
	$(CC) -c $(SRC_DIR)/main.c -o $(BUILD_DIR)/main.o

matrix.o: $(SRC_DIR)/matrix.c $(SRC_DIR)/matrix.h
	$(CC) -c $(SRC_DIR)/matrix.c -o $(BUILD_DIR)/matrix.o

seed.o: $(SRC_DIR)/seed.c $(SRC_DIR)/seed.h
	$(CC) -c $(SRC_DIR)/seed.c -o $(BUILD_DIR)/seed.o

ui.o: $(SRC_DIR)/ui.c $(SRC_DIR)/ui.h
	$(CC) -c $(SRC_DIR)/ui.c -o $(BUILD_DIR)/ui.o

clean:
	rm $(BUILD_DIR)/*.o

install:
	cp -f dface ~/bin/apps
	chmod +x ~/bin/apps/dface
