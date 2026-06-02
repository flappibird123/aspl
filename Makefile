# ===== Project config =====
CC := clang
STD := -std=c11

TARGET := aspl 

SRC_DIR := src
BUILD_DIR := build

# ===== Auto-discover sources recursively =====
SRCS := $(shell find $(SRC_DIR) -name "*.c")

# Map .c -> .o in build/
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

# ===== Flags =====
CFLAGS := -Wall -Wextra  $(STD) -I$(SRC_DIR) -Isrc/
DEPFLAGS := -MMD -MP

# ===== Default target =====
all: $(TARGET)

# ===== Link step =====
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

# ===== Compile step (recursive build dirs) =====
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

# ===== Include auto-generated dependency files =====
-include $(DEPS)

# ===== Cleanup =====
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

re: clean all

.PHONY: all clean re