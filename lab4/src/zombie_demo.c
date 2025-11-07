#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    pid_t child_pid;

    // 1. Создание дочернего процесса
    child_pid = fork();

    if (child_pid > 0) {
        // Код родительского процесса
        printf("Родительский процесс (PID: %d). Дочерний процесс (PID: %d).\n", getpid(), child_pid);
        printf("Родитель будет спать 60 секунд. Зомби-процесс будет виден в течение этого времени.\n");
        // Родительский процесс просто "спит" и не вызывает wait()
        sleep(60); 
        printf("Родительский процесс завершил сон.\n");

    } 
    else  (child_pid == 0) {
        // Код дочернего процесса
        printf("Дочерний процесс (PID: %d) завершается немедленно.\n", getpid());
        // 2. Дочерний процесс завершается
        exit(0); 

    }
    return 0;
}