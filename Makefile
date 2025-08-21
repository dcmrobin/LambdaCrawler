# Compiler
CXX := g++
CXXFLAGS := -Wall -std=c++17 -O2 -I./SDL2/include -Isrc
LDFLAGS := -L./SDL2/lib
LDLIBS := -lSDL2 -lSDL2_image -lSDL2_ttf

# Executable and assets
EXEC := build/LambdaCrawler.exe
ASSETS_SRC := assets
ASSETS_DEST := build/assets

# DLLs
SDL_DLL_SOURCE := SDL2/bin/SDL2.dll
SDL_IMAGE_DLL_SOURCE := SDL2/bin/SDL2_image.dll
SDL_TTF_SOURCE := SDL2/bin/SDL2_ttf.dll
GCC_DLL_SOURCE := /mingw64/bin/libgcc_s_seh-1.dll
STDCPP_DLL_SOURCE := /mingw64/bin/libstdc++-6.dll
PTHREAD_DLL_SOURCE := /mingw64/bin/libwinpthread-1.dll
GRAPHITE_DLL_SOURCE := /mingw64/bin/libgraphite2.dll
HARFBUZZ_DLL_SOURCE := /mingw64/bin/libharfbuzz-0.dll
GLIB_DLL_SOURCE := /mingw64/bin/libglib-2.0-0.dll
FREETYPE_DLL_SOURCE := /mingw64/bin/libfreetype-6.dll
BZ2_DLL_SOURCE := /mingw64/bin/libbz2-1.dll
BROTLI_DEC_DLL_SOURCE := /mingw64/bin/libbrotlidec.dll
BROTLI_COMMON_DLL_SOURCE := /mingw64/bin/libbrotlicommon.dll
PNG_DLL_SOURCE := /mingw64/bin/libpng16-16.dll
ZLIB_DLL_SOURCE := /mingw64/bin/zlib1.dll
INTL_DLL_SOURCE := /mingw64/bin/libintl-8.dll
PCRE2_DLL_SOURCE := /mingw64/bin/libpcre2-8-0.dll
ICONV_DLL_SOURCE := /mingw64/bin/libiconv-2.dll

# Source files
SRCS := $(shell find src -name '*.cpp')

# Default target
all: $(EXEC) copy_dll copy_assets

$(EXEC): $(SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

copy_dll:
	@echo "Copying DLLs..."
	@cp "$(SDL_DLL_SOURCE)" "build/" || echo "WARNING: SDL2.dll not found"
	@cp "$(SDL_IMAGE_DLL_SOURCE)" "build/" || echo "WARNING: SDL2_image.dll not found"
	@cp "$(SDL_TTF_SOURCE)" "build/" || echo "WARNING: SDL2_ttf.dll not found"
	@cp "$(GCC_DLL_SOURCE)" "build/" || echo "WARNING: libgcc_s_seh-1.dll not found"
	@cp "$(STDCPP_DLL_SOURCE)" "build/" || echo "WARNING: libstdc++-6.dll not found"
	@cp "$(PTHREAD_DLL_SOURCE)" "build/" || echo "WARNING: libwinpthread-1.dll not found"
	@cp "$(GRAPHITE_DLL_SOURCE)" "build/" || echo "WARNING: libgraphite2.dll not found"
	@cp "$(HARFBUZZ_DLL_SOURCE)" "build/" || echo "WARNING: libharfbuzz-0.dll not found"
	@cp "$(GLIB_DLL_SOURCE)" "build/" || echo "WARNING: libglib-2.0-0.dll not found"
	@cp "$(FREETYPE_DLL_SOURCE)" "build/" || echo "WARNING: libfreetype-6.dll not found"
	@cp "$(BZ2_DLL_SOURCE)" "build/" || echo "WARNING: libbz2-1.dll not found"
	@cp "$(BROTLI_DEC_DLL_SOURCE)" "build/" || echo "WARNING: libbrotlidec.dll not found"
	@cp "$(BROTLI_COMMON_DLL_SOURCE)" "build/" || echo "WARNING: libbrotlicommon.dll not found"
	@cp "$(PNG_DLL_SOURCE)" "build/" || echo "WARNING: libpng16-16.dll not found"
	@cp "$(ZLIB_DLL_SOURCE)" "build/" || echo "WARNING: zlib1.dll not found"
	@cp "$(INTL_DLL_SOURCE)" "build/" || echo "WARNING: libintl-8.dll not found"
	@cp "$(PCRE2_DLL_SOURCE)" "build/" || echo "WARNING: libpcre2-8-0.dll not found"
	@cp "$(ICONV_DLL_SOURCE)" "build/" || echo "WARNING: libiconv-2.dll not found"

copy_assets:
	@echo "Copying assets..."
	@mkdir -p $(ASSETS_DEST)
	@cp -r $(ASSETS_SRC)/* $(ASSETS_DEST)/ || echo "WARNING: No assets found"

clean:
	rm -rf build

run: all
	@echo "Running LambdaCrawler..."
	cd build && ./LambdaCrawler.exe

.PHONY: all clean run copy_dll copy_assets