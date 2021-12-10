CC = gcc
CFLAGS =
BUILD_DIR = build
BIN_DIR = .
SRC_DIR = src

# create build dir
$(shell mkdir -p $(BUILD_DIR))

# target: dependencies
# 	  action

gol: matrix.o node.o main.o
	$(CC) $(BUILD_DIR)/matrix.o $(BUILD_DIR)/node.o $(BUILD_DIR)/main.o $(CFLAGS) -o $(BIN_DIR)/gol

main.o: $(SRC_DIR)/main.c 
	$(CC) -c $(SRC_DIR)/main.c -o $(BUILD_DIR)/main.o

matrix.o: $(SRC_DIR)/matrix.c $(SRC_DIR)/matrix.h
	$(CC) -c $(SRC_DIR)/matrix.c -o $(BUILD_DIR)/matrix.o

node.o: $(SRC_DIR)/node.c $(SRC_DIR)/node.h
	$(CC) -c $(SRC_DIR)/node.c -o $(BUILD_DIR)/node.o

clean:
	rm $(BUILD_DIR)/*.o

install:
	cp -f dface ~/bin/apps
	chmod +x ~/bin/apps/dface
