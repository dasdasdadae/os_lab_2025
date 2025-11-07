#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>      
#include <signal.h> 

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

//  Глобальные переменные состояния для функции обработки сигнала 
pid_t *global_pids = NULL;
int global_pnum = 0;
int global_timeout_seconds = -1;

// ФУНКЦИЯ-ОБРАБОТЧИК (Будет вызвана по SIGALRM)
void timeout_handler(int signum) {
    if (signum == SIGALRM) {
        printf("\n Родитель (PID %d): Тайм-аут (%d с) истек! Отправка SIGKILL всем детям...\n", getpid(), global_timeout_seconds > 0 ? global_timeout_seconds : 0);
        
        for (int i = 0; i < global_pnum; i++) {
            if (global_pids[i] > 0) {
                // Отправка SIGKILL
                if (kill(global_pids[i], SIGKILL) == 0) {
                    printf("  -> SIGKILL отправлен дочернему процессу %d.\n", global_pids[i]);
                } else {
                    // Если kill не удался, процесс уже мог завершиться
                    if (errno == ESRCH) {
                        printf("  -> Дочерний процесс %d уже завершился.\n", global_pids[i]);
                    } else {
                        perror("kill failed");
                    }
                }
            }
        }
        
        // Немедленное и безопасное завершение родительского процесса
        _exit(1); // Завершаемся с ошибкой (1), так как работа не была выполнена
    }
}

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;
// Инициализируем переменную для timeout -1
  int timeout_seconds = -1;

  int **pipefd = NULL;
  if (!with_files) {
    pipefd = malloc(pnum * sizeof(int*));
    for (int i = 0; i < pnum; i++) {
      pipefd[i] = malloc(2 * sizeof(int));
      pipe(pipefd[i]);
    }
  }

  while (true) {
    int current_optind = optind ? optind : 1;

static struct option options[] = {
            {"seed", required_argument, 0, 0},
            {"array_size", required_argument, 0, 0},
            {"pnum", required_argument, 0, 0},
            {"by_files", no_argument, 0, 'f'},
            // Добавляем --timeout 
            {"timeout", required_argument, 0, 0}, 
            {0, 0, 0, 0}
        };
    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:  // --seed
            seed = atoi(optarg);
            if (seed <= 0) {
                printf("Error: seed must be a positive number\n");
                return 1;
            }
            break;
          case 1:  // --array_size
            array_size = atoi(optarg);
            if (array_size <= 0) {
                printf("Error: array_size must be a positive number\n");
                return 1;
            }
            break;
          case 2:  // --pnum
            pnum = atoi(optarg);
            if (pnum <= 0) {
                printf("Error: pnum must be a positive number\n");
                return 1;
            }
            break;
          case 3:  // --by_files
            with_files = true;
            break;
          default:
            printf("Index %d is out of options\n", option_index);
          // Обработка --timeout (option_index = 4) 
          case 4:  
              timeout_seconds = atoi(optarg);
              if (timeout_seconds <= 0) {
                  printf("Error: timeout must be a positive number\n");
                  return 1;
              }
              break;
        }
        break;
      case 'f':
        with_files = true;
        break;
      case '?':
        break;
      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

// Инициализация глобального массива PID 
  if (pnum > 0) {
      global_pids = malloc(sizeof(pid_t) * pnum);
      global_pnum = pnum;
  }
  if (timeout_seconds > 0){
    global_timeout_seconds = timeout_seconds;
  }
  
  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  // Создаем pipes если используем pipe (не файлы)
  if (!with_files) {
    pipefd = malloc(pnum * sizeof(int*));
    for (int i = 0; i < pnum; i++) {
      pipefd[i] = malloc(2 * sizeof(int));
      pipe(pipefd[i]);
    }
  }

  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      active_child_processes += 1;
      if (child_pid == 0) {
        //  ДОЧЕРНИЙ ПРОЦЕСС
        
        // Вычисляем границы для этого процесса
        int chunk_size = array_size / pnum;
        int start = i * chunk_size;
        int end = (i == pnum - 1) ? array_size : (i + 1) * chunk_size;
        
        // Вычисляем min/max в своей части массива
        struct MinMax result = GetMinMax(array, start, end);
        
        if (with_files) {
          // Записываем в файл
          char filename[50];
          sprintf(filename, "minmax_%d.txt", getpid());
          FILE *f = fopen(filename, "w");
          fprintf(f, "%d %d", result.min, result.max);
          fclose(f);
        } else {
          // Отправляем через pipe
          close(pipefd[i][0]); // Закрываем read-end в ребенке
          write(pipefd[i][1], &result, sizeof(result));
          close(pipefd[i][1]);
        }
        return 0;
      }
    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  if (!with_files) {
    for (int i = 0; i < pnum; i++) {
      close(pipefd[i][1]);
    }
  }

// Установка таймера и обработчика ---
  if (timeout_seconds > 0) {
      if (signal(SIGALRM, timeout_handler) == SIG_ERR) {
          perror("signal error");
          // Продолжаем, но без таймера
      } else {
          alarm(timeout_seconds);
          printf("Parent: Таймер запущен на %d секунд.\n", timeout_seconds);
      }
  }
  // Ждем завершения всех детей
  while (active_child_processes > 0) {
    wait(NULL);
    active_child_processes -= 1;
  }

//  Отмена таймера и очистка PID
    if (timeout_seconds > 0) {
        alarm(0); // Отменяем таймер, если все дети завершились нормально
    }
    free(global_pids);


  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    struct MinMax child_result;
    
    if (with_files) {
      char filename[50];
      sprintf(filename, "minmax_*.txt");
      FILE *f = fopen(filename, "r");
      if (f) {
        fscanf(f, "%d %d", &child_result.min, &child_result.max);
        fclose(f);
      }
    } else {
      read(pipefd[i][0], &child_result, sizeof(child_result));
      close(pipefd[i][0]);
    }

    if (child_result.min < min_max.min) min_max.min = child_result.min;
    if (child_result.max > min_max.max) min_max.max = child_result.max;
  }

  if (!with_files) {
    for (int i = 0; i < pnum; i++) {
      free(pipefd[i]);
    }
    free(pipefd);
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}