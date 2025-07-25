# Compiler
CXX := g++
CXXFLAGS := -Wall -std=c++17 -O2 -I./SDL2/include -Isrc
LDFLAGS := -L./SDL2/lib
LDLIBS := -lSDL2

# Output executable
EXEC := build/LambdaCrawler.exe

# DLLs
SDL_DLL_SOURCE := SDL2/bin/SDL2.dll
GCC_DLL_SOURCE := /mingw64/bin/libgcc_s_seh-1.dll
STDCPP_DLL_SOURCE := /mingw64/bin/libstdc++-6.dll
PTHREAD_DLL_SOURCE := /mingw64/bin/libwinpthread-1.dll

# Source files
SRCS := $(shell find src -name '*.cpp')

# Default target
all: $(EXEC) copy_dll

$(EXEC): $(SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

copy_dll:
	@echo "Copying DLLs..."
	@cp "$(SDL_DLL_SOURCE)" "build/" || echo "WARNING: SDL2.dll not found"
	@cp "$(GCC_DLL_SOURCE)" "build/" || echo "WARNING: libgcc_s_seh-1.dll not found"
	@cp "$(STDCPP_DLL_SOURCE)" "build/" || echo "WARNING: libstdc++-6.dll not found"
	@cp "$(PTHREAD_DLL_SOURCE)" "build/" || echo "WARNING: libwinpthread-1.dll not found"

clean:
	rm -rf build

run: all
	@echo "Running LambdaCrawler..."
	cd build && ./LambdaCrawler.exe

.PHONY: all clean run copy_dll
