#!/bin/bash

echo "=== Building string reversal libraries ==="
mkdir bin

# Компиляция объектных файлов
gcc -c revert_string.c -o bin/revert_string.o
gcc -c main.c -o bin/main.o

# Создание статической библиотеки
echo "Creating static library..."
ar rcs bin/librevert.a bin/revert_string.o

# Статическая линковка
echo "Linking static executable..."
gcc bin/main.o ./bin/librevert.a -o bin/statically_linked

# Компиляция объектных файлов
gcc -c -fPIC revert_string.c -o bin/revert_string_pic.o
gcc -c main.c -o bin/main_pic.o


# Создание динамической библиотеки
echo "Creating dynamic library..."
gcc -shared bin/revert_string_pic.o -o bin/librevert.so


# Динамическая линковка
echo "Linking dynamic executable..."
gcc bin/main_pic.o -L./bin -lrevert -Wl,-rpath,./bin -o bin/dynamically_linked

echo "=== Build complete ==="
