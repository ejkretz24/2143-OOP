CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra

# SDL2 flags for include directories
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LDFLAGS := $(shell sdl2-config --libs)

TARGET = program
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(SDL_LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(SDL_CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)     

clean:
	rm -f $(OBJ) $(TARGET)
