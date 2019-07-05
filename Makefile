CC = g++
CFLAGS = -Wall -Iinclude/ -g -std=c++17

all: main.o vec3.o ray.o sphere.o world.o util.o material.o
	$(CC) $^ -o raytracer

%.o: src/%.cpp
	$(CC) -c $< -o $@ $(CFLAGS)
	
