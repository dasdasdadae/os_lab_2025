#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../lab3/src/utils.h"
#include <pthread.h>

struct SumArgs {
  int *array;
  int begin;
  int end;
};

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

int main(int argc, char **argv) {
  uint32_t threads_num = 0;
  uint32_t array_size = 0;
  uint32_t seed = 0;

  if (argc == 4) {
    threads_num = atoi(argv[1]);
    array_size = atoi(argv[2]);
    seed = atoi(argv[3]);
  } else {
    fprintf(stderr, "Ошибка: Ожидается 3 позиционных аргумента.\n");
    fprintf(stderr, "Использование: %s <num_threads> <array_size> <seed>\n", argv[0]);
    return 1;
  }

  pthread_t threads[threads_num];

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);

  // Вычисляем размер части массива для каждого потока
  int chunk_size = array_size / threads_num;
  
  struct SumArgs args[threads_num];
  for (uint32_t i = 0; i < threads_num; i++) {
    // Инициализируем параметры для каждого потока
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
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  int total_sum = 0;
  for (uint32_t i = 0; i < threads_num; i++) {
    int sum = 0;
    pthread_join(threads[i], (void **)&sum);
    total_sum += sum;
  }

  free(array);
  printf("Total: %d\n", total_sum);
  return 0;
}