#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    // Аргументы для sequential_min_max
    char *seed = "42";
    char *array_size = "100";
    
    // Можно передать аргументы через командную строку
    if (argc >= 3) {
        seed = argv[1];
        array_size = argv[2];
    }
        
    int id = fork();
    
    if (id == 0) {
        // ДОЧЕРНИЙ ПРОЦЕСС
        char *myargs[4];
        myargs[0] = "./sequential_min_max";  // программа
        myargs[1] = seed;                    // первый аргумент
        myargs[2] = array_size;              // второй аргумент  
        myargs[3] = NULL;                    // завершающий NULL
        
        execvp(myargs[0], myargs);
        
        // Если дошли сюда - ошибка (должен измениться процесс)
        perror("execvp failed");
        exit(1);
    }
    else if (id > 0) {
        // РОДИТЕЛЬСКИЙ ПРОЦЕСС
        int status;
        status = wait(NULL);
    }
    else {
        perror("fork failed");
        return 1;
    }
    
    return 0;
}