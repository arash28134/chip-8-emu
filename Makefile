# set the compiler
CC := gcc

# sdl2-config commands
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LIBS := $(shell sdl2-config --libs)

# set the compiler flags
CFLAGS := -ggdb3 -O0 --std=c99 -Wall -lm -Iinclude $(SDL_CFLAGS) 

# linker flags
LFLAGS := $(SDL_LIBS) 

# add source files here
SRCS := src/graphics.c src/main.c src/rom.c
HDRS := include/graphics.h include/rom.h

# generate names of object files
OBJS := $(SRCS:.c=.o)

# name of executable
EXEC := chip8 

# default recipe
all: $(EXEC)
	
# recipe for building the final executable
$(EXEC): $(OBJS) Makefile
	$(CC) $(OBJS) $(CFLAGS) $(LFLAGS) -o $@ 

# pattern rule for object files
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

# recipe to clean the workspace
clean:
	rm -f $(EXEC) $(OBJS)

.PHONY: all clean
