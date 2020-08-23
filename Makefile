CC = g++
CFLAGS = -Wall -Wextra -Iinclude/ -Iexternal/include/ -std=c++17 -g -fopenmp -O3
LDFLAGS = -lpthread -g -fopenmp -O3

all: main.o ray.o sphere.o world.o util.o material.o camera.o trianglemesh.o box.o pdf.o shape.o light.o
	$(CC) $^ -o raytracer $(LDFLAGS)

%.o: src/%.cpp
	$(CC) -c $< -o $@ $(CFLAGS)
