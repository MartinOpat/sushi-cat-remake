###################### CatTest
# Makefile for Sushi Cat Game using SDL2, SDL2_gfx, and Box2D

# Compiler settings
CXX = g++
CXXFLAGS = -Wall -std=c++17 `sdl2-config --cflags` -Ibox2d-main/include -I/usr/include
LDFLAGS = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lSDL2_gfx -Lbox2d-main/build/bin -lbox2d -L/usr/lib -lGL -lGLU

# Target executable
TARGET = SushiCat

# Source files
SOURCES = cat_test.cpp  # Assuming this is the correct source file
OBJECTS = $(SOURCES:.cpp=.o)

# Default build target
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

# To obtain object files
%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

# To remove generated files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Phony targets to prevent conflicts
.PHONY: all clean

# To print variables for debugging the Makefile
print-%  : ; @echo $* = $($*)
