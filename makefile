# Compiler
CC = clang

# Files
SRCS = $(wildcard src/*.c)

# Compiler flags
CFLAGS = -Wall -Werror -Wextra -O2

# Libraries
LIBS = -lm -lSDL2

# Executable name
NAME = raycaster

# Build
all: $(SRCS)
	$(CC) $(SRCS) $(CFLAGS) $(LIBS) -o $(NAME) 
