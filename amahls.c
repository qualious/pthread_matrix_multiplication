#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>

typedef enum { false, true } bool;
typedef void (*fptr)(int);

void init_matrices();
double stopwatch(fptr, int);
void create_and_execute(int);
void* deploy_worker(void*);
void multiply(int);
void print_matrix(int**);
void free_mem();

int size = 1000;
int thread_count = 0;
int **first_matrix, **second_matrix, **result_matrix;
bool verbose = false;
bool matrix_info = false;

int
main(int argc, char* argv[]) {

    if (argc == 2 && strcmp(argv[1], "-h") == 0) {
        puts("A simple application that utilizes threads in order to do matrix multiplication.");
        puts("Will output the calculated matrix in verbose mode. It'll only output thread finish time without verbose mode.");
        puts("You can use -v flag to enable verbose mode!");
        puts("You can use -m flag to enable printing matrices!");
        return EXIT_SUCCESS;
    }

    if (argc == 2) {
        if(strcmp(argv[1], "-v") == 0) {
            verbose = true;
            puts("Verbose mode enabled.");
        }
    }

    if (argc == 3 && (strcmp(argv[1], "-v") == 0 || strcmp(argv[2], "-v") == 0)) {
        verbose = true;
        return EXIT_SUCCESS;
    }

    if (argc == 3 && (strcmp(argv[1], "-m") == 0 || strcmp(argv[2], "-m") == 0)) {
        matrix_info = true;
        return EXIT_SUCCESS;
    }

    init_matrices();
    verbose && puts("Initialization complete.");

    if (matrix_info) {
        puts("First matrix:");
        print_matrix(first_matrix);
        puts("Second matrix:");
        print_matrix(second_matrix);
    }

    for (int i = 1; i < 50; ++i) {
        thread_count = i;
        printf("\n======================================\n");
        printf("Time it took when thread count is %d : %f ",
                i, stopwatch(create_and_execute, i));
        printf("\n======================================\n");
    }

    free_mem();
    return EXIT_SUCCESS;
}

void
init_matrices() {
    int i, j;
    first_matrix = (int**)malloc(size * sizeof(int *));;
    second_matrix = (int**)malloc(size * sizeof(int *));;
    result_matrix = (int**)malloc(size * sizeof(int *));;

    for(i = 0; i < size; ++i) {
        first_matrix[i] = malloc(size * sizeof(int));
        second_matrix[i] = malloc(size * sizeof(int));
        result_matrix[i] = malloc(size * sizeof(int));
    }

    srand(time(0)+clock()+random());

    for(i = 0; i < size; i++) {
        for(j = 0; j < size; j++) {
            first_matrix[i][j] = rand()%100;
            second_matrix[i][j] = rand()%100;
        }
    }
}

double
stopwatch(fptr func, int count) {
    struct timeval start, finish;
    double elapsed = 0;
    gettimeofday(&start, NULL);
    func(count);
    gettimeofday(&finish, NULL);
    elapsed = (finish.tv_sec - start.tv_sec) * 1000.0;
    elapsed += (finish.tv_usec - start.tv_usec) / 1000.0;
    return elapsed / 1000.0;
}

void
create_and_execute(count) {
    pthread_t* threads = (pthread_t*) malloc(sizeof(pthread_t) * count);

    if (threads == NULL) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }

    verbose && puts("Creating threads.");
    for (size_t i = 0; i < count; ++i) {
        if (pthread_create(&threads[i], NULL, deploy_worker, (void *)i) != 0){
            fprintf(stderr, "Can't create thread %zu\n", i);
            exit(EXIT_FAILURE);
        }
    }
    verbose && puts("Joining threads.");
    for (int i = 0; i < count; i++) {
        pthread_join(threads[i], NULL);
    }

    if (matrix_info) {
        puts("Result is: ");
        print_matrix(result_matrix);
    }

    verbose && puts("Clearing result matrix.");
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            result_matrix[i][j] = 0;
        }
    }

    free(threads);
    verbose && puts("Task complete!");
}

void*
deploy_worker(void *i) {
    int thread_id = (int)i;
    multiply(thread_id);
    pthread_exit(0);
}

void
multiply(int thread_id) {
    int i, j, k;
    double segment = (double)size/(double)thread_count;
    int segment_size = ceil(segment);
    int start = thread_id * segment_size;
    int end = (thread_id + 1) * segment_size - 1;
    if (end >= size) end = size - 1;

    if (verbose) {
        // printf("size: %d, thread_count: %d, segment: %f\n", size, thread_count, segment);
        printf("thread_id: %d, segment_size: %d\n", thread_id, segment_size);
        printf("start: %d, end: %d\n", start, end);
    }

    for(i = start; i <= end; i++) {
        for(j = 0; j < size; j++) {
            result_matrix[i][j] = 0;
            for(k = 0; k < size; k++) {
                result_matrix[i][j] += first_matrix[i][k] * second_matrix[k][j];
            }
        }
    }
}

void
print_matrix(int **matrix) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

void
free_mem() {
    int i = 0;
    for(i = 0; i < size; ++i){
        free((void *)first_matrix[i]);
    }
    free((void *)first_matrix);

    for(i = 0; i < size; ++i){
        free((void *)second_matrix[i]);
    }
    free((void *)second_matrix);

    for(i = 0; i < size; ++i){
        free((void *)result_matrix[i]);
    }
    free((void *)result_matrix);

}
