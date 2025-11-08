#ifndef SUM_LIB_H
#define SUM_LIB_H

#include <stdint.h>

// Структура для передачи аргументов в поток
struct SumArgs {
    int *array;
    int begin;
    int end;
};

// Функция для последовательного вычисления суммы части массива
int Sum(const struct SumArgs *args);

// Функция для параллельного вычисления суммы массива
int ParallelSum(int *array, int array_size, int threads_num);

#endif // SUM_LIB_H