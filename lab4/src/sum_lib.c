#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "sum_lib.h"

int Sum(const struct SumArgs *args) {
    int sum = 0;
    for (int i = args->begin; i < args->end; i++) {
        sum += args->array[i];  
    }
    return sum;
}

void *ThreadSum(void *args) {
    struct SumArgs *sum_args = (struct SumArgs *)args;
    return (void *)(size_t)Sum(sum_args);
}

int ParallelSum(int *array, int array_size, int threads_num) {
    if (threads_num <= 0 || array == NULL || array_size <= 0) {
        return 0;
    }

    // Если потоков больше чем элементов в массиве, ограничиваем количество потоков
    if (threads_num > array_size) {
        threads_num = array_size;
    }

    pthread_t threads[threads_num];
    struct SumArgs args[threads_num];

    // Вычисляем размер части массива для каждого потока
    int chunk_size = array_size / threads_num;
    
    // Создаем потоки
    for (uint32_t i = 0; i < threads_num; i++) {
        args[i].array = array;
        args[i].begin = i * chunk_size;
        
        if (i == threads_num - 1) {
            // Последний поток получает все оставшиеся элементы
            args[i].end = array_size;
        } else {
            // Обычные потоки получают ровный кусок
            args[i].end = (i + 1) * chunk_size;
        }
        
        if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
            fprintf(stderr, "Error: pthread_create failed!\n");
            return -1;
        }
    }

    // Собираем результаты
    int total_sum = 0;
    for (uint32_t i = 0; i < threads_num; i++) {
        int sum = 0;
        pthread_join(threads[i], (void **)&sum);
        total_sum += sum;
    }

    return total_sum;
}