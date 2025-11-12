# SSH Blog Makefile
# Builds the blog viewer program

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -O2 -D_POSIX_C_SOURCE=200809L
LDFLAGS = -lncurses

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TARGET = $(BIN_DIR)/blog_viewer

# Default target
all: $(TARGET)

# Link object files into executable
$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Clean complete"

# Install to system (requires sudo)
install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/blog_viewer
	@echo "Installed to /usr/local/bin/blog_viewer"

# Uninstall from system
uninstall:
	rm -f /usr/local/bin/blog_viewer
	@echo "Uninstalled from /usr/local/bin/blog_viewer"

# Run the program (for testing)
run: $(TARGET)
	$(TARGET)

# Debug build
debug: CFLAGS += -g -O0 -DDEBUG
debug: clean $(TARGET)
	@echo "Debug build complete"

# Show help
help:
	@echo "SSH Blog Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all        - Build the blog viewer (default)"
	@echo "  clean      - Remove build artifacts"
	@echo "  run        - Build and run the program"
	@echo "  debug      - Build with debug symbols"
	@echo "  install    - Install to /usr/local/bin (requires sudo)"
	@echo "  uninstall  - Remove from /usr/local/bin"
	@echo "  help       - Show this help message"

.PHONY: all clean install uninstall run debug help
