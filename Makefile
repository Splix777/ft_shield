# Compiler and flags
GCC = gcc
CFLAGS = -Wall -Wextra -Werror -O3 -obfuscator-llvm
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

# Colors
GREEN = \033[32m
RESET = \033[0m

# Default target
all: $(TARGET) pack

unpacked: $(TARGET)

# Link object files into the target binary
$(TARGET): $(OBJ_FILES)
	@printf "Linking %-42s" "$(NAME)"
	@$(GCC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	@echo "$(GREEN)[ OK ]$(RESET)"
	@echo "$(GREEN)Binary size: $$(wc -c < $@) bytes$(RESET)"

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	@$(GCC) $(CFLAGS) -I$(INCLUDE_DIR) -c -o $@ $<

pack:
	@printf "Packing %-42s" "$(TARGET)"
	@(cd $(PACKER_DIR) && make) >/dev/null 2>&1
	@./$(PACKER_DIR)/$(PACKER) -f $(TARGET) -o $(TARGET)_packed
	@rm -f $(TARGET)
	@mv $(TARGET)_packed $(TARGET)
	@echo "$(GREEN)[ OK ]$(RESET)"
	@echo "Packed binary: $(TARGET)"


# Clean up build artifacts
clean:
	@printf "Cleaning objects... %-2s"
	@rm -rf $(OBJ_DIR)
	@echo "$(GREEN)[ OK ]$(RESET)"

fclean: clean
	@printf "Cleaning binaries... %-1s"
	@(sudo ./clean) >/dev/null 2>&1
	@rm -f $(TARGET)
	@(cd $(PACKER_DIR) && make fclean) >/dev/null 2>&1
	@echo "$(GREEN)[ OK ]$(RESET)"

re: fclean all

debug: CFLAGS += -g3 -DDEBUG -fsanitize=address
debug: re

.PHONY: all pack clean fclean re debug
