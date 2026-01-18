CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wextra -g

BUILD_DIR = ./build

INCLUDES = -I.

DEMO_TARGET = $(BUILD_DIR)/demo

LIBS = ../vendor/raylib/src/libraylib.a

all: $(DEMO_TARGET)

$(DEMO_TARGET): ./demo/main.cpp $(BUILD_DIR)/aabbtree.o
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@ $(LIBS)

$(BUILD_DIR)/aabbtree.o: ./aabbtree.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean all

.PHONY: all clean rebuild
