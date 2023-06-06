# Makefile for Game Project

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -I/opt/homebrew/Cellar/libwebsockets/4.3.2/include -I/opt/homebrew/opt/openssl@3/include

# Directories
LIBWEBSOCKETS_INCLUDE_DIR = /opt/homebrew/Cellar/libwebsockets/4.3.2/include
OPENSSL_INCLUDE_DIR = /opt/homebrew/opt/openssl@3/include
LIBWEBSOCKETS_LIB_DIR = /opt/homebrew/Cellar/libwebsockets/4.3.2/lib
OPENSSL_LIB_DIR = /opt/homebrew/opt/openssl@3/lib

# Libraries
LIBS = -lpthread -lwebsockets

# Source files
SOURCES = queueStruct.c game.c websocket_server.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Executable name
EXECUTABLE = game

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ -I $(LIBWEBSOCKETS_INCLUDE_DIR) -I $(OPENSSL_INCLUDE_DIR) -L $(LIBWEBSOCKETS_LIB_DIR) -L $(OPENSSL_LIB_DIR) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(EXECUTABLE) $(OBJECTS)
