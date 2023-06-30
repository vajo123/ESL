# OpenCV flags
OPENCV_LIBS=$(shell pkg-config --libs opencv4)
OPENCV_CFLAGS=$(shell pkg-config --cflags opencv4)

# Compiler to use
CC=g++

# Compiler flags
CFLAGS=-c $(OPENCV_CFLAGS)

# Linker flags
LDFLAGS=-lsystemc $(OPENCV_LIBS)

# Source files
SOURCES=main.cpp memory.cpp cpu.cpp intcon.cpp dma.cpp ip.cpp ../spec/readsrt.cpp

# Object files
OBJECTS=$(SOURCES:.cpp=.o)

# Executable name
EXECUTABLE=main

# Default target
all: $(EXECUTABLE)

# Build target
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# Compile target
.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

# Clean target
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
