# 1. Compiler and Flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wno-missing-braces -O2

# 2. Output File Name
TARGET = physics_engine

# 3. Source Files
SRCS = main.cpp

# 4. OS Detection & Raylib Flags
ifeq ($(OS),Windows_NT)
	# Windows (MinGW)
	LDFLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm
	TARGET := $(TARGET).exe
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		# Linux
		LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
	endif
	ifeq ($(UNAME_S),Darwin)
		# macOS
		LDFLAGS = -lraylib -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
	endif
endif

# 5. Build Rules
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)