# ===============================
# Memory Management Simulator
# ===============================

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./include

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Source files (UPDATED PATHS)
SOURCES = \
	$(SRC_DIR)/main.cpp \
	$(SRC_DIR)/normal_allocator/normal_allocator.cpp \
	$(SRC_DIR)/buddy_allocator/buddy_allocator.cpp \
	$(SRC_DIR)/cache/cache.cpp \
	$(SRC_DIR)/vm/vm.cpp

# Object files
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Target executable
TARGET = $(BIN_DIR)/memsim

# ===============================
# Build rules
# ===============================

all: $(TARGET)

# Link step
$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $(TARGET)
	@echo "Build complete: $(TARGET)"

# Compile step (auto-creates folders)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Clean complete"

# Run
run: all
	./$(TARGET)

.PHONY: all clean run
