# Compiler
CXX := g++
CXXFLAGS := -Wall -std=c++17 -O2 -I./SDL2/include -Isrc
LDFLAGS := -L./SDL2/lib
LDLIBS := -lSDL2 -lSDL2_image -lSDL2_ttf

# Executable and assets
EXEC := build/LambdaCrawler.exe
ASSETS_SRC := assets
ASSETS_DEST := build/assets

# Source files
SRCS := $(shell find src -name '*.cpp')

# Target icon files
ICON_ICO := assets/sprites/icon.ico
RC_FILE := build/icon.rc
RES_OBJ := build/icon.o

# Default target
all: $(EXEC) copy_dll copy_dll_auto copy_assets

$(EXEC): $(SRCS)
	@mkdir -p build
	@# Check if icon.ico exists, compile resource object if it does
	@if [ -f "$(ICON_ICO)" ]; then \
		echo "Building executable..."; \
		echo "MAINICON ICON \"../$(ICON_ICO)\"" > $(RC_FILE); \
		windres $(RC_FILE) -o $(RES_OBJ); \
		$(CXX) $(CXXFLAGS) $^ $(RES_OBJ) -o $@ $(LDFLAGS) $(LDLIBS); \
	else \
		echo "Warning: $(ICON_ICO) not found. Building without embedded system icon..."; \
		$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS); \
	fi

# Copy all SDL DLLs from local SDL2/bin
copy_dll: $(EXEC)
	@echo "Copying SDL2 DLLs..."
	@cp SDL2/bin/*.dll build/ || echo "WARNING: No SDL2 DLLs found"

# Auto-detect dependent DLLs using ldd
copy_dll_auto: $(EXEC)
	@echo "Scanning for dependent DLLs..."
	@ldd $(EXEC) > build/ldd.log
	@echo "Copying dependent MinGW DLLs..."
	@grep -o '/[^ ]*\.dll' build/ldd.log | grep '/mingw64/bin/' | while read dll; do \
		echo "  -> $$dll"; \
		cp "$$dll" build/; \
	done

copy_assets:
	@echo "Copying assets..."
	@mkdir -p $(ASSETS_DEST)
	@cp -r $(ASSETS_SRC)/* $(ASSETS_DEST)/ || echo "WARNING: No assets found"

clean:
	rm -rf build

run: all
	@echo "Running LambdaCrawler..."
	cd build && ./LambdaCrawler.exe

.PHONY: all clean run copy_dll copy_dll_auto copy_assets
