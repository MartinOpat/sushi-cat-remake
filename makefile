###################### SushiCat Game
# Makefile for Sushi Cat Game using SDL2, SDL2_gfx, and Box2D

# Compiler settings
CXX = g++
CXXFLAGS = -g -Wall -std=c++17 `sdl2-config --cflags` -Ibox2d-main/include -I/usr/include -Isrc
LDFLAGS = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lSDL2_gfx -Lbox2d-main/build/bin -lbox2d -L/usr/lib -lGL -lGLU

# Target executable
TARGET = SushiCatRemake

# Source files and objects
SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:src/%.cpp=%.o)

# Default build target
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

# To obtain object files
%.o: src/%.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

# To remove generated files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Phony targets to prevent conflicts
.PHONY: all clean

# To print variables for debugging the Makefile
print-%  : ; @echo $* = $($*)
