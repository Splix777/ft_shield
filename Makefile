# Compiler and flags
GCC = gcc
CFLAGS = -Wall -Wextra -Werror -O3 -static -obfuscator-llvm
LDFLAGS = 

# Directories
SRC_DIR = src
INCLUDE_DIR = include
OBJ_DIR = obj
TARGET = ft_shield
PACKER_DIR = woody_woodpacker
PACKER = woody_woodpacker

# Find all .c files in the src directory and its subdirectories
SRC_FILES = $(shell find $(SRC_DIR) -name '*.c')

# Generate corresponding .o files in the obj directory
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

# Default target
all: $(TARGET) pack

# Link object files into the target binary
$(TARGET): $(OBJ_FILES)
	@$(GCC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	@$(GCC) $(CFLAGS) -I$(INCLUDE_DIR) -c -o $@ $<

pack:
	@cd $(PACKER_DIR) && make
	@./$(PACKER_DIR)/$(PACKER) -f $(TARGET) -o $(TARGET)_packed
	@rm -f $(TARGET)
	@mv $(TARGET)_packed $(TARGET)
	@echo "Packed binary: $(TARGET)"

# Clean up build artifacts
clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@sudo ./clean
	@rm -f $(TARGET)
	@cd $(PACKER_DIR) && make fclean

re: fclean all

debug: CFLAGS += -g3 -DDEBUG
debug: re

.PHONY: all pack clean fclean re debug
