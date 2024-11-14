CC = gcc
CFLAGS = -Wall -Wextra -g -I./raylib-5.0_linux_amd64/include/
LIB_PATH = -L./raylib-5.0_linux_amd64/lib/
LIBS = -l:libraylib.a -lm -ldl -lpthread

BIN_DIR = ./bin
SRC_DIR = ./src

breakout: 
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) ./engine.c ./collision.c  -o $(BIN_DIR)/engine $(LIB_PATH) $(LIBS)

run:
	./bin/engine

# breakout: 
# 	mkdir -p $(BIN_DIR)
# 	$(CC) $(CFLAGS) ./physics.c ./collision.c  -o $(BIN_DIR)/physics $(LIB_PATH) $(LIBS)

# run:
# 	./bin/physics

clean:
	rm -rf $(BIN_DIR)

.PHONY: clean $(BIN_DIR)
