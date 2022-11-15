#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// VARIABLES
int delta, n, m, t, flag = 0, print = 1;
float **new_array, **old_array;
pthread_barrier_t br, br_main;

// FUNCTION THREAD
void * calc ();

// FUNCTION CALC VALUE
float calc_val(int, int);

// FUNCTION PRINT
void print_array(float **);

// FUNCTION REWRITE
void rewrite_array();

int main(int argc, char **argv) {
  int n_threads, thread, *idx;
  pthread_t * handler_threads;
  FILE * file;

  if (argc < 2) {
    printf("Enter file\n");
    exit(EXIT_FAILURE);
  }

  file = fopen(argv[1], "r");
  if (!file) {
    printf("Error in fopen\n");
    exit(EXIT_FAILURE);
  }

  // GET PARAMETERS
  fscanf(file, "%d %d %d %d", &n, &m, &n_threads, &t);

  if (n % n_threads != 0) {
    printf("rows [%d] is not divisible between threads [%d]\n", n, n_threads);
    exit(EXIT_FAILURE);
  }

  delta = n / n_threads;

  printf("Value n is: %d\n", n);
  printf("Value m is: %d\n", m);
  printf("Value n_threads is: %d\n", n_threads);
  printf("Value t is: %d\n", t);
  
  // ASIGNMENT MEMORY SPACE
  handler_threads = malloc(sizeof(pthread_t) * n_threads);

  old_array = malloc(sizeof(float *)*n);
  for (int i = 0; i < m; i++) {
    old_array[i] = malloc(sizeof(float)*m);
  }

  new_array = malloc(sizeof(float *)*n);
  for (int i = 0; i < m; i++) {
    new_array[i] = malloc(sizeof(float)*m);
  }

  // READING ARRAY
  for (int r = 0; r < n; r++) {
    for (int c = 0; c < m; c++) {
      fscanf(file, "%f", &old_array[r][c]);      
    }
  }

  // PRINT INITIAL INTERVAL
  printf("\n");
  printf("Interval initial:\n");
  print_array(old_array);

  pthread_barrier_init(&br, NULL, n_threads+1);
  pthread_barrier_init(&br_main, NULL, n_threads+1);

  // CREATE THREADS
  for (thread = 0; thread < n_threads; thread++) {
    idx = malloc(sizeof(int));
    *idx = thread;
    pthread_create(&handler_threads[thread], NULL, calc, idx); 
  }

  // PRINT NEW ARRAY
  for (int i = 0; i<t; i++) {
    pthread_barrier_wait(&br);
    printf("\n");
    printf("Interval [%d]:\n", i+1);
    print_array(new_array);
    rewrite_array();
    pthread_barrier_wait(&br_main);
  }

  // WAITING THREADS
  for (thread = 0; thread < n_threads; thread++) {
    pthread_join(handler_threads[thread], NULL); 
  }

  fclose(file);
  free(old_array);
  free(new_array);
  pthread_barrier_destroy(&br);
  pthread_barrier_destroy(&br_main);

  return EXIT_SUCCESS;
}

// FUNCTION THREAD
void * calc( void * id ) {
  int id_thread = *((int *) id);
  int to = (id_thread + 1) * delta;
  int from = to - delta;
  float new_value;
  for(int i=0; i<t; i++) {
    for (int row = from; row < to; row++) {
      for (int column = 0; column < m; column++) {
        if (row == 0 || column == 0 || row == n-1 || column == m-1) {
          new_value = old_array[row][column];
        } else {
          new_value = calc_val(row, column);
        }
        new_array[row][column] = new_value;
      }
    }
    pthread_barrier_wait(&br);
    pthread_barrier_wait(&br_main);
  }
  free(id);
  return NULL;
}

// CALC VALUE
float calc_val(int r, int c) {
  float new = (old_array[r][c-1] + old_array[r-1][c] + old_array[r][c+1] + old_array[r+1][c])/4;
  return new;
}

// PRINT ARRAY
void print_array(float **array) {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      if(array[i][j] < array[0][0]){
        printf("%.2f  ", array[i][j]);
      } else {
        printf("%.2f ", array[i][j]);
      }
    }
    printf("\n");
  }
}

// REWRITE ARRAY
void rewrite_array() {
  for (int r = 0; r < n; r++) {
    for (int c = 0; c < m; c++) {
      old_array[r][c] = new_array[r][c];
    }
  }
}
