# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pthread

# Пути
LAB3_DIR = ../../lab3/src
LAB4_DIR = ../../lab4/src
BIN_DIR = bin
EXEC_DIR = exec

# Цели по умолчанию
all: $(EXEC_DIR)/parallel_min_max $(EXEC_DIR)/process_memory $(EXEC_DIR)/parallel_sum

# Создание папок
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(EXEC_DIR):
	mkdir -p $(EXEC_DIR)

# Программа parallel_min_max
$(EXEC_DIR)/parallel_min_max: $(BIN_DIR)/parallel_min_max.o $(BIN_DIR)/utils.o $(BIN_DIR)/find_min_max.o | $(EXEC_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(BIN_DIR)/parallel_min_max.o: $(LAB3_DIR)/parallel_min_max.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BIN_DIR)/utils.o: $(LAB3_DIR)/utils.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BIN_DIR)/find_min_max.o: $(LAB3_DIR)/find_min_max.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Программа process_memory
$(EXEC_DIR)/process_memory: $(BIN_DIR)/process_memory.o | $(EXEC_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

$(BIN_DIR)/process_memory.o: $(LAB4_DIR)/process_memory.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Программа parallel_sum
$(EXEC_DIR)/parallel_sum: $(BIN_DIR)/parallel_sum.o $(BIN_DIR)/utils.o $(BIN_DIR)/sum_lib.o | $(EXEC_DIR)
	$(CC) $(LDFLAGS) -pthread -o $@ $^

$(BIN_DIR)/parallel_sum.o: parallel_sum.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -I$(LAB3_DIR) -c -o $@ $<

$(BIN_DIR)/sum_lib.o: sum_lib.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -I$(LAB3_DIR) -c -o $@ $<

# Очистка
clean:
	rm -rf $(BIN_DIR) $(EXEC_DIR)

# Псевдоцели
.PHONY: all clean