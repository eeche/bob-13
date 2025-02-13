# Makefile

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -o

# Target executable name
TARGET = nfqnl_test

# Source files
SRCS = nfqnl_test.c

# Libraries
LIBS = -lnetfilter_queue

# Build target
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(TARGET) $(SRCS) $(LIBS)

# Clean up build files
clean:
	rm -f $(TARGET)

