# set the compiler
CC := gcc

# sdl2-config commands
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LIBS := $(shell sdl2-config --libs)

# set the compiler flags
CFLAGS := --std=c99 -Wall -lm -Iinclude $(SDL_CFLAGS)

# linker flags
LFLAGS := $(SDL_LIBS) -lSDL2_mixer 

# add source files here
SRCS := src/main.c src/graphics.c src/disassembler.c
HDRS := include/graphics.h include/disassembler.h

# generate names of object files
OBJS := $(SRCS:.c=.o)

# name of executable
EXEC := chip8 

# recipe for building the final executable
all: $(OBJS) Makefile
	$(CC) $(OBJS) $(CFLAGS) $(LFLAGS) -o $(EXEC)

# pattern rule for object files
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

debug: $(OBJS) Makefile
	$(CC) -g $(SRCS) $(CFLAGS) $(LFLAGS) -o $(EXEC)

# recipe to clean the workspace
clean:
	rm -f $(EXEC) $(OBJS)

.PHONY: all clean
