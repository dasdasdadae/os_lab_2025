#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> // Для sleep

// 1. Объявление двух ресурсов (мьютексов)
pthread_mutex_t resourceA;
pthread_mutex_t resourceB;

// Функция, выполняемая Потоком 1
void *thread1_func(void *arg) {
    printf("Поток 1: Попытка захватить Resource A...\n");
    pthread_mutex_lock(&resourceA);
    printf("Поток 1: *** Resource A захвачен ***\n");

    // Искусственная задержка, чтобы дать Потоку 2 время захватить Resource B
    sleep(1); 

    printf("Поток 1: Попытка захватить Resource B...\n");
    // Здесь Поток 1 будет заблокирован, ожидая Resource B, который держит Поток 2
    pthread_mutex_lock(&resourceB); 
    printf("Поток 1: *** Resource B захвачен (УСПЕХ) ***\n"); // Эта строка НЕ будет достигнута

    // Освобождение ресурсов (только в теории)
    pthread_mutex_unlock(&resourceB);
    pthread_mutex_unlock(&resourceA);
    printf("Поток 1: Завершил работу.\n");

    return NULL;
}

// Функция, выполняемая Потоком 2
void *thread2_func(void *arg) {
    printf("Поток 2: Попытка захватить Resource B...\n");
    pthread_mutex_lock(&resourceB);
    printf("Поток 2: *** Resource B захвачен ***\n");

    printf("Поток 2: Попытка захватить Resource A...\n");
    // Здесь Поток 2 будет заблокирован, ожидая Resource A, который держит Поток 1
    pthread_mutex_lock(&resourceA); 
    printf("Поток 2: *** Resource A захвачен (УСПЕХ) ***\n"); // Эта строка НЕ будет достигнута

    // Освобождение ресурсов (только в теории)
    pthread_mutex_unlock(&resourceA);
    pthread_mutex_unlock(&resourceB);
    printf("Поток 2: Завершил работу.\n");

    return NULL;
}

int main() {
    pthread_t t1, t2;

    // Инициализация мьютексов
    pthread_mutex_init(&resourceA, NULL);
    pthread_mutex_init(&resourceB, NULL);
    
    printf("--- Запуск сценария Deadlock ---\n");
    
    // Создание потоков
    pthread_create(&t1, NULL, thread1_func, NULL);
    pthread_create(&t2, NULL, thread2_func, NULL);

    // Ожидание завершения потоков.
    // Программа зависнет здесь, так как потоки никогда не завершатся.
    pthread_join(t1, NULL); 
    pthread_join(t2, NULL); 
    
    printf("--- Deadlock устранен (НЕ ДОСТИЖИМО) ---\n");

    // Очистка мьютексов
    pthread_mutex_destroy(&resourceA);
    pthread_mutex_destroy(&resourceB);

    return 0;
}