# Compiler and flags
GCC = gcc
CFLAGS = -Wall -Wextra -Werror -O3 -flto -fvisibility=hidden -frename-registers -fno-common -fomit-frame-pointer -fstack-protector-all -fstrict-aliasing -fno-strict-overflow -fno-diagnostics-color -funroll-loops -D_FORTIFY_SOURCE=2 -obfuscator-llvm -static
LDFLAGS = -s

# Directories
SRC_DIR = src
INCLUDE_DIR = include
OBJ_DIR = obj
TARGET = ft_shield
PACKER_DIR = woody_woodpacker
PACKER = woody_woodpacker
CLEAN_BINARY = clean

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
	@printf "Linking %-42s" "$(TARGET)"
	@$(GCC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	@echo "$(GREEN)[ OK ]$(RESET)"
	@echo "$(GREEN)Binary size: $$(wc -c < $@) bytes$(RESET)"

# Compile source files into object files with obfuscation flags
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
	@strip --strip-all $(TARGET)
	@upx --ultra-brute --lzma --no-backup --strip-relocs  $(TARGET)
	@echo "Packed binary: $(TARGET)"

# Clean up build artifacts
clean:
	@printf "Cleaning objects... %-2s"
	@rm -rf $(OBJ_DIR)
	@echo "$(GREEN)[ OK ]$(RESET)"

fclean: clean
	@printf "Cleaning binaries... %-1s"
	@if [ ! -f $(CLEAN_BINARY) ]; then \
		$(GCC) $(CFLAGS) clean.c -o $(CLEAN_BINARY); \
	fi
	@sudo ./$(CLEAN_BINARY) >/dev/null 2>&1
	@rm -f $(CLEAN_BINARY) $(TARGET)
	@(cd $(PACKER_DIR) && make fclean) >/dev/null 2>&1
	@echo "$(GREEN)[ OK ]$(RESET)"

re: fclean all

debug: CFLAGS += -g3 -DDEBUG
debug: re

.PHONY: all unpacked pack clean fclean re debug

# Compiler flags and meanings:
# -Wall: Enable all warnings
# -Wextra: Enable extra warnings
# -Werror: Treat warnings as errors
# -O3: Optimize for speed
# -flto: Enable link-time optimization. Enables Link-Time Optimization (LTO), which optimizes the whole program at the link stage, making the binary harder to reverse-engineer.
# -fvisibility=hidden: Reduces the exposure of internal symbols to avoid unnecessary symbol exposure.
# -frename-registers: Renames CPU registers to make the code harder to follow.
# -fno-common: Disallows the use of common symbols, which are shared between multiple object files.
# -fomit-frame-pointer: Omit the frame pointer for functions that don't need it. This reduces the size of the binary and can make stack analysis harder.
# -fstack-protector-all: Enable stack protection for all functions. This adds stack canaries to functions to prevent stack smashing attacks. Adds protection to all functions to defend against buffer overflows.
# -fstrict-aliasing: Optimizes the code based on strict aliasing rules, improving performance but making the program harder to understand.
# -fno-strict-overflow: Prevents certain overflow checks, further complicating reverse engineering.
# -fno-diagnostics-color: Disables color output for diagnostics, making it harder to read compiler messages.
# -funroll-loops: Unrolls loops to improve performance, but can make the code harder to understand.
# -D_FORTIFY_SOURCE=2: Enables additional security features in the standard library, such as buffer overflow protection.
# -obfuscator-llvm: Enable LLVM obfuscation passes. This flag enables LLVM obfuscation passes, which can make the code harder to understand.
# -static: Link the binary statically. This makes the binary larger but can make it harder to analyze.

# Linker flags and meanings:
# -s: Strip symbols from the binary. This removes debugging information and makes the binary harder to analyze.

# After compiling we strip the binary to remove debugging information and reduce the binary size. This makes the binary harder to analyze and reverse-engineer.
# strip --strip-all $(TARGET)

# We then us UPX to compress the binary using the best compression algorithm. This reduces the binary size further and makes it harder to analyze.
# @upx --ultra-brute --lzma --no-backup --strip-relocs  $(TARGET)
# --ultra-brute: This is an extreme compression method. It compresses the binary even more than --best but can be much slower.
# --lzma: Use the LZMA compression algorithm. This is the best compression algorithm but can be slower.
# --no-backup: Do not create a backup of the original binary.
# --strip-relocs: Strips relocation information from the binary, further reducing its size and making it harder to analyze.
