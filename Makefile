CC = g++
CFLAGS = -Wall -Wextra -Iinclude/ -Iexternal/include/ -std=c++17 -fopenmp
LDFLAGS = -lpthread -fopenmp

all: main.o ray.o sphere.o world.o util.o material.o camera.o trianglemesh.o box.o pdf.o shape.o light.o imagetexture.o
	$(CC) $^ -o raytracer $(LDFLAGS)

%.o: src/%.cpp
	$(CC) -c $< -o $@ $(CFLAGS)
