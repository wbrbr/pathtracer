CC = g++
# CFLAGS = -Wall -Iinclude/ -g -std=c++17 -fsanitize=address,undefined
# LDFLAGS = -fsanitize=address,undefined
CFLAGS = -Wall -Iinclude/ -Iexternal/include/ -std=c++17 -g -O3
LDFLAGS = -lpthread -O3

all: main.o vec3.o ray.o sphere.o world.o util.o material.o camera.o trianglemesh.o box.o pdf.o shape.o
	$(CC) $^ -o raytracer $(LDFLAGS)

%.o: src/%.cpp
	$(CC) -c $< -o $@ $(CFLAGS)
	

run: all
	./raytracer
