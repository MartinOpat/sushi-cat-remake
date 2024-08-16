###################### SushiCat Game
# Makefile for Sushi Cat Game using SDL2, SDL2_gfx, and Box2D

# Compiler settings
CXX = g++
CXXFLAGS = -g -Wall -std=c++17 `sdl2-config --cflags` -Ibox2d-main/include -I/usr/include -Isrc
LDFLAGS = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lSDL2_gfx -Lbox2d-main/build/bin -lbox2d -L/usr/lib -lGL -lGLU

# Windows compiler settings
WIN_CXX = x86_64-w64-mingw32-g++
WIN_SDL2_BASE = SDL2/x86_64-w64-mingw32
WIN_SDL2_IMAGE_BASE = SDL2_image/x86_64-w64-mingw32
WIN_SDL2_GFX_BASE = SDL2_gfx/mingw64
WIN_SDL2_TTF_BASE = SDL2_ttf/x86_64-w64-mingw32

WIN_BOX2D_INCLUDE = box2d-win/mingw64/include
WIN_BOX2D_LIB = box2d-win/mingw64/bin

WIN_CXXFLAGS = -Wall -std=c++17 \
               -I$(WIN_SDL2_BASE)/include \
               -I$(WIN_SDL2_BASE)/include/SDL2 \
               -I$(WIN_SDL2_IMAGE_BASE)/include \
               -I$(WIN_SDL2_GFX_BASE)/include \
			   -I$(WIN_SDL2_TTF_BASE)/include \
               -I$(WIN_BOX2D_INCLUDE) \
               -Isrc

WIN_LDFLAGS = -L$(WIN_SDL2_BASE)/lib \
              -L$(WIN_SDL2_IMAGE_BASE)/lib \
              -L$(WIN_SDL2_GFX_BASE)/lib \
			  -L$(WIN_SDL2_TTF_BASE)/lib \
              -L$(WIN_BOX2D_LIB) \
              -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_gfx \
              -lbox2d -lmingw32 -lopengl32 -lglu32 -mwindows

# Target executable
TARGET = SushiCatRemake
WIN_TARGET = SushiCatRemake.exe

# Source files and objects
SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:src/%.cpp=%.o)
WIN_OBJECTS = $(SOURCES:src/%.cpp=%_win.o)

# Default build target
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

# Windows build target
win: $(WIN_TARGET)

$(WIN_TARGET): $(WIN_OBJECTS)
	$(WIN_CXX) $(WIN_OBJECTS) -o $@ $(WIN_LDFLAGS)

# To obtain object files
%.o: src/%.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

# To obtain object files for Windows
%_win.o: src/%.cpp
	$(WIN_CXX) -c $(WIN_CXXFLAGS) $< -o $@

# To remove generated files
clean:
	rm -f $(OBJECTS) $(WIN_OBJECTS) $(TARGET) $(WIN_TARGET)

# Phony targets to prevent conflicts
.PHONY: all clean win

# To print variables for debugging the Makefile
print-%  : ; @echo $* = $($*)
