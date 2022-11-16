/* Taller hilos - Jacobi
 * 
 * Brian Castaño - 2018214054
 * Isaac Mejia - 2018114046
 * Omar Fandiño - 2018114023
 *
 * Grupo: 2
 *
 * */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// GLOBAL VARIABLES
int delta, n, m, t_interval;
float **new_array, **old_array;
pthread_barrier_t br_thread;

// FUNCTION THREAD
void * thread_calc();

// FUNCTION CALC VALUES
void calc_values(int, int);

// FUNCTION PRINT
void print_array(float **);

// FUNCTION REWRITE
void rewrite_array();

// FUNCTION ERROR HANDLING
void error_handling(int);

// FUNCTION MAIN
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
  fscanf(file, "%d %d %d %d", &n, &m, &n_threads, &t_interval);

  error_handling(n_threads);

  delta = n / n_threads;

  printf("Value rows is: %d\n", n);
  printf("Value columns is: %d\n", m);
  printf("Value number threads is: %d\n", n_threads);
  printf("Value interval is: %d\n", t_interval);
  
  // ASIGNMENT MEMORY SPACE
  handler_threads = malloc(sizeof(pthread_t) * n_threads);

  old_array = (float**)malloc(sizeof(float *)*n);
  for (int i = 0; i < n; i++) {
    old_array[i] = (float*)malloc(sizeof(float)*m);
  }

  new_array = (float**)malloc(sizeof(float *)*n);
  for (int i = 0; i < n; i++) {
    new_array[i] = (float*)malloc(sizeof(float)*m);
  }

  // READING ARRAY
  for (int r = 0; r < n; r++) {
    for (int c = 0; c < m; c++) {
      fscanf(file, "%f", &old_array[r][c]);      
    }
  }

  // PRINT INITIAL INTERVAL
  printf("\n");
  printf(" --  Interval initial  -- \n\n");
  print_array(old_array);

  pthread_barrier_init(&br_thread, NULL, n_threads+1);

  // CREATE THREADS
  for (thread = 0; thread < n_threads; thread++) {
    idx = malloc(sizeof(int));
    *idx = thread;
    pthread_create(&handler_threads[thread], NULL, thread_calc, idx); 
  }

  // PRINT NEW ARRAY
  for (int i = 0; i<t_interval; i++) {
    printf("\n --  Calculating interval [%d]  -- \n\n", i + 1);
    pthread_barrier_wait(&br_thread); // STARTING THREAD
    pthread_barrier_wait(&br_thread); // WAITING THREAD
    printf("\n");
    printf("Result:\n");
    print_array(new_array);
    rewrite_array();
  }

  // WAITING THREADS
  for (thread = 0; thread < n_threads; thread++) {
    pthread_join(handler_threads[thread], NULL); 
  }

  fclose(file);
  free(old_array);
  free(new_array);
  pthread_barrier_destroy(&br_thread);

  return EXIT_SUCCESS;
}

// FUNCTION THREAD CALC
void * thread_calc( void * id ) {
  int id_thread = *((int *) id), from, to;
  to = (id_thread + 1) * delta;
  from = to - delta;
  float new_value;
  for(int i=0; i<t_interval; i++) {
    pthread_barrier_wait(&br_thread); // WAITING MAIN THREAD
    printf("-> thread %d\n", id_thread+1);
    calc_values(from, to);
    pthread_barrier_wait(&br_thread); // STARTING MAIN THREAD
  }
  free(id);
  return NULL;
}

// CALC VALUES
void calc_values(int from, int to) {
  float new_value;
  for (int row = from; row < to; row++) {
    for (int column = 0; column < m; column++) {
      if (row == 0 || column == 0 || row == n-1 || column == m-1) {
        new_value = old_array[row][column];
      } else {
        new_value = (old_array[row][column-1] + old_array[row-1][column] + old_array[row][column+1] + old_array[row+1][column])/4;
      }
      new_array[row][column] = new_value;
    }
  }
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
  printf("\n");
}

// REWRITE ARRAY
void rewrite_array() {
  for (int r = 0; r < n; r++) {
    for (int c = 0; c < m; c++) {
      old_array[r][c] = new_array[r][c];
    }
  }
}

// ERROR HANDLING
void error_handling(int n_threads) {
  if (n == 0 || m == 0) {
    printf("Row = %d, Columns = %d. Both rows and columns must be greater than 0\n", n, m);
    exit(EXIT_FAILURE);
  }
  if (n_threads == 0) {
    printf("Number threads = 0, need at least 1 thread to calculate interval(s)\n");
    exit(EXIT_FAILURE);
  }
  if (n % n_threads != 0) {
    printf("Rows [%d] is not divisible between threads [%d]\n", n, n_threads);
    exit(EXIT_FAILURE);
  }
  if (t_interval == 0) {
    printf("Interval(s) = 0, need at least 1 interval\n");
    exit(EXIT_FAILURE);
  }
}
