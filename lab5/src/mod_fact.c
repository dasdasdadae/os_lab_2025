#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h> 	
#include <errno.h>
#include <pthread.h>

// -------------------
// Глобальные переменные для хранения параметров
long k_value = 0; 	 	// Факториал k!
int pnum_value = 0; 	// Количество потоков
long mod_value = 0; 	// Модуль

// Глобальные переменные для результата и синхронизации
long total_result = 1; // Общий результат 
pthread_mutex_t mutex; // Мьютекс для защиты total_result
// -------------------

// Структура для передачи данных в поток
typedef struct {
    int thread_id; 
    long start;     // Начальное число диапазона (включительно)
    long end;       // Конечное число диапазона (включительно)
} thread_data_t;

// Функция, выполняемая потоком
void *calculate_sum(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;
    long local_product = 1;

    // Вычисление произведения в своем диапазоне по модулю mod_value
    for (long i = data->start; i <= data->end; i++) {
        local_product = (local_product * i) % mod_value;
    }

    // Критическая секция: обновление общего результата с использованием мьютекса
    pthread_mutex_lock(&mutex);
    
    // Обновление общего результата
    total_result = (total_result * local_product) % mod_value;

    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}
// -------------------

int main(int argc, char *argv[]) {
    int opt;
    int long_index = 0;
    
    // Структура, описывающая длинные опции
    static struct option long_options[] = {
        // { "имя", "требует_аргумент", "флаг", "возвращаемое_значение/символ" }
        {"pnum", required_argument, 0, 'p'},
        {"mod",  required_argument, 0, 'm'},
        {"k", required_argument, 0, 'k'},
        {0, 0, 0, 0} // Конец списка
    };

    // Строка коротких опций: p, m, k, каждая требует аргумент (:)
    while ((opt = getopt_long(argc, argv, "p:m:k:", long_options, &long_index)) != -1) {
        switch(opt){
            case 'p':
                // Используем strtol для лучшей обработки ошибок и long
                pnum_value = (int)strtol(optarg, NULL, 10);
                break; 
            case 'm':
                mod_value = strtol(optarg, NULL, 10);
                break; 
            case 'k':
                k_value = strtol(optarg, NULL, 10);
                break; 
            case '?': // Неизвестная опция или отсутствующий аргумент
                fprintf(stderr, "Использование: %s -k <число> --pnum=<потоки> --mod=<модуль>\n", argv[0]);
                return EXIT_FAILURE;
            // Добавлена обработка ошибок getopt_long
        }
    }

    // Проверка обязательных параметров
    if (k_value <= 0 || pnum_value <= 0 || mod_value <= 0) {
        fprintf(stderr, "Ошибка: все параметры (k, pnum, mod) должны быть положительными числами.\n");
        fprintf(stderr, "Использование: %s -k <число> --pnum=<потоки> --mod=<модуль>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // 1. Инициализация мьютекса
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("Ошибка инициализации мьютекса");
        return EXIT_FAILURE;
    }

    // 2. Подготовка данных для потоков
    
    pthread_t threads[pnum_value]; // Массив для идентификаторов потоков
    thread_data_t thread_data[pnum_value]; // Массив для данных потоков
    
    long chunk_size = k_value / pnum_value;
    long remainder = k_value % pnum_value;
    long current_start = 1;

    // Разделение работы
    for (int i = 0; i < pnum_value; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].start = current_start;
        // Распределяем оставшиеся элементы (remainder) поровну
        thread_data[i].end = current_start + chunk_size - 1 + (i < remainder ? 1 : 0);
        current_start = thread_data[i].end + 1;
        
        // Создание потоков
        // Проверяем результат pthread_create на ошибку
        int rc = pthread_create(&threads[i], NULL, calculate_sum, &thread_data[i]);
        if (rc) {
            fprintf(stderr, "Ошибка при создании потока %d: %s\n", i, strerror(rc));
            pthread_mutex_destroy(&mutex);
            return EXIT_FAILURE;
        }
    }

    //Ожидание завершения потоков
    for (int i = 0; i < pnum_value; i++) {
        int rc = pthread_join(threads[i], NULL);
        if (rc) {
            fprintf(stderr, "Ошибка при ожидании завершения потока %d: %s\n", i, strerror(rc));
        }
    }
    
    printf("Вычислено: %ld! mod %ld = %ld\n", k_value, mod_value, total_result);

    pthread_mutex_destroy(&mutex);

    return EXIT_SUCCESS;
}