//Tanmay Parikh
//Milan Patel

#include <stdio.h>
#include <pthread.h>

#define ROWSA 18
#define COLSA 16

#define ROWSB 16
#define COLSB 18

int A[ROWSA][COLSA];
int B[ROWSB][COLSB];
int C[ROWSA][COLSB];

struct row_col{
  int row, col;
};

typedef struct row_col row_col_t;

void initArrays() {
  
  int i, j;
  int rows, cols;

  for (i = 0; i < ROWSA; i++) {
    for (j = 0; j < COLSA; j++) {
      A[i][j] = (i + 1) + (j + 1);
    }
  }
  
  for (i = 0; i < ROWSB; i++) {
    for (j = 0; j < COLSB; j++) {
      B[i][j] = (i + 1) + 2 * (j + 1);
    }
  }

}

void *compute_C_ij(void *arg) {

  row_col_t *index = (row_col_t*) arg;
  int sum = 0;
  int row = index->row;
  int col = index->col;
  int j;
  
  for(j = 0; j < COLSA; j++) {
    sum = sum + A[row][j] * B[j][col];
  }
  
  C[row][col] = sum;
  
  pthread_exit(NULL);

}

int main() {

  initArrays();
  int i, j;

  pthread_t computation_threads[ROWSA][COLSB];
  row_col_t indexes[ROWSA][COLSB];
  
  for (i = 0; i < ROWSA; i++) {
    for (j = 0; j < COLSB; j++) {
      indexes[i][j].row = i;
      indexes[i][j].col = j;
      pthread_create(&computation_threads[i][j], NULL, &compute_C_ij, &indexes[i][j]);
    }
  }
  
  for (i = 0; i < ROWSA; i++) {
    for (j = 0; j < COLSB; j++) {
        pthread_join(computation_threads[i][j], NULL);
    }
  }
  
  for (i = 0; i < ROWSA; i++) {
    for (j = 0; j < COLSB; j++) {
      printf("%d\t", C[i][j]);
    }
      printf("\n");
  }

  pthread_exit(NULL);

}
