#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#include "../../lab3/src/utils.h"
#include "sum_lib.h"

int main(int argc, char **argv) {
    uint32_t threads_num = 0;
    uint32_t array_size = 0;
    uint32_t seed = 0;

    // Парсинг аргументов командной строки
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--threads_num") == 0 && i + 1 < argc) {
            threads_num = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--array_size") == 0 && i + 1 < argc) {
            array_size = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            seed = atoi(argv[++i]);
        }
    }

    // Проверка корректности аргументов
    if (threads_num == 0 || array_size == 0) {
        fprintf(stderr, "Ошибка: Неверные аргументы.\n");
        fprintf(stderr, "Использование: %s --threads_num <num> --array_size <num> --seed <num>\n", argv[0]);
        return 1;
    }

    // Генерация массива (не входит в замер времени)
    int *array = malloc(sizeof(int) * array_size);
    if (array == NULL) {
        fprintf(stderr, "Ошибка: Не удалось выделить память для массива.\n");
        return 1;
    }
    GenerateArray(array, array_size, seed);

    // Замер времени начала вычислений
    struct timespec start, finish;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Параллельное вычисление суммы
    int total_sum = ParallelSum(array, array_size, threads_num);

    // Замер времени окончания вычислений
    clock_gettime(CLOCK_MONOTONIC, &finish);

    // Вычисление времени выполнения
    double elapsed_time = (finish.tv_sec - start.tv_sec);
    elapsed_time += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

    // Вывод результатов
    printf("Total sum: %d\n", total_sum);
    printf("Elapsed time: %.6f seconds\n", elapsed_time);

    free(array);
    return 0;
}