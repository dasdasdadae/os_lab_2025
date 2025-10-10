#!/bin/bash

mkdir bin

gcc -c find_min_max.c -o bin/find_min_max.o
gcc -c sequential_min_max.c -o bin/sequential_min_max.o
gcc -c utils.c -o bin/utils.o

gcc -o bin/min_max_program bin/find_min_max.o bin/sequential_min_max.o bin/utils.o
