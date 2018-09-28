# Compiler: gcc for C/C++ program
CC = gcc

# Compiler flags:
# -g: adds debugging info to .exe file
# -Wall: turns on most compiler warnings
CFLAGS = -Wall

# List of src files:
SRC = client.c
OBJ = $(SRC:.c=.o)

# Build target executable:
TARGET = client

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

clean:
	$(RM) *.o $(shell find . -type f -executable ! -name "*.*")
