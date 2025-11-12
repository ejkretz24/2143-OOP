# --------------------------
# Compiler and flags
# --------------------------
CXX := g++
CXXFLAGS := -Wall -std=c++17

# Default: no special libraries (for console programs)
LDFLAGS :=

# --------------------------
# Targets
# --------------------------
TARGET := program01
SRC := main.cpp

# --------------------------
# Configuration: GUI vs Console
# Set GUI=1 if you want SDL window, leave empty for console
# --------------------------
ifeq ($(GUI),1)
    PKG_CFLAGS := $(shell pkg-config --cflags sdl2)
    PKG_LIBS   := $(shell pkg-config --libs sdl2)
    # Remove -Dmain=SDL_main if pkg-config adds it
    CXXFLAGS += $(filter-out -Dmain=SDL_main,$(PKG_CFLAGS))
    LDFLAGS  += $(PKG_LIBS) -lSDL2main -mwindows
endif

# --------------------------
# Build rules
# --------------------------
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
