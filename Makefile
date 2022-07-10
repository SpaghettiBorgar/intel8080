CC=gcc
TARGET := 8080
SRC_DIR := ./src
BUILD_DIR := ./bin
CFLAGS := -g

normal:
	$(eval SRCS := $(shell find $(SRC_DIR) -name '*.c'))
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(TARGET) $(SRCS)

# for easier debugging of macros
expanded:
	$(eval SRCS := $(shell find $(SRC_DIR) -name '*.c' -and -not -name '*cpu.c'))
	mkdir -p ./tmp $(BUILD_DIR)
	cpp -P $(SRC_DIR)/cpu.c ./tmp/cpu_expanded.c
	clang-format -i ./tmp/cpu_expanded.c
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(TARGET) tmp/cpu_expanded.c $(SRCS)

clean:
	rm -rf ./tmp $(BUILD_DIR)