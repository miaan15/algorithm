CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wextra -g
INCLUDE = -I.

TARGET = a

SRCS = main.cpp arena/arena.cpp arrlist/arrlist.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $^

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

rebuild: clean all

.PHONY: all run clean rebuild
