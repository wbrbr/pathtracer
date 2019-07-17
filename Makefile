CC = g++
CFLAGS = -Wall -Iinclude/ -g -std=c++17 -fsanitize=address
LDFLAGS = -fsanitize=address

all: main.o vec3.o ray.o sphere.o world.o util.o material.o camera.o trianglemesh.o box.o
	$(CC) $^ -o raytracer $(LDFLAGS)

%.o: src/%.cpp
	$(CC) -c $< -o $@ $(CFLAGS)
	
