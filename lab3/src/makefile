CC=gcc
CFLAGS=-I.

all: sequential_min_max execute_seq_min_max parallel_min_max 


sequential_min_max: sequential_min_max.o utils.o find_min_max.o
	$(CC) -o sequential_min_max sequential_min_max.o utils.o find_min_max.o $(CFLAGS)

execute_seq_min_max: execute_seq_min_max.o utils.o find_min_max.o
	$(CC) -o execute_seq_min_max execute_seq_min_max.o utils.o find_min_max.o $(CFLAGS)

parallel_min_max: parallel_min_max.o utils.o find_min_max.o
	$(CC) -o parallel_min_max parallel_min_max.o utils.o find_min_max.o $(CFLAGS)

sequential_min_max.o: sequential_min_max.c utils.h find_min_max.h
	$(CC) -c sequential_min_max.c $(CFLAGS)

execute_seq_min_max.o: execute_seq_min_max.c utils.h find_min_max.h
	$(CC) -c execute_seq_min_max.c $(CFLAGS)

parallel_min_max.o: parallel_min_max.c utils.h find_min_max.h
	$(CC) -c parallel_min_max.c $(CFLAGS)

utils.o: utils.c utils.h
	$(CC) -c utils.c $(CFLAGS)

find_min_max.o: find_min_max.c find_min_max.h utils.h
	$(CC) -c find_min_max.c $(CFLAGS)

clean:
	rm -f *.o sequential_min_max execute_seq_min_max parallel_min_max

