CC = gcc
CFLAGS = -Wall -Wextra -std=c11 `pkg-config --cflags glfw3 glew`
LDFLAGS = `pkg-config --libs glfw3 glew` -lm
SRC = main.c setup-opengl/setup-opengl.c draw/draw.c physics/physics.c
OBJ = $(SRC:.c=.o)
TARGET = program 

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
