#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

#define aR 18
#define bR 16

#define aC 16
#define bC 18



int a[aR][aC];
int b[bR][bC];
int result[aR][bC];

struct matrix_info {

	int row;
	int column;
};

typedef struct matrix_info matrix_info_t;

void *compute_c_ij(void *arg) {

	int sum = 0;

	matrix_info_t *matrix_info = (matrix_info_t *)arg;

	for (int k=0; k < bR ; k++)
	{
		sum = sum + a[matrix_info->row][k]*b[k][matrix_info->column];
	}

	result[matrix_info->row][matrix_info->column] = sum ;

	pthread_exit(NULL);
}

void printMatrix(size_t rows, size_t	columns, int matrix[rows][columns]);
void initializeMatrix();

int main(void) {

	pthread_t threads[aR][bC];
	matrix_info_t matrix_info [aR][bC];

		//initialize matrices
	initializeMatrix();

	for (int i=0 ; i < aR ; i++)
	{
		for (int j=0 ; j < bC ; j++)
		{
			matrix_info[i][j].row = i;
			matrix_info[i][j].column = j;
			pthread_create(&threads[i][j], NULL, &compute_c_ij, &matrix_info[i][j]);
		}
	}


	for (int i=0 ; i < aR ; i++)
	{
		for (int j=0 ; j < bC ; j++)
		{
			pthread_join(&threads[i][j],NULL);
		}
	}

	printMatrix(aR, bC, result);

}

void printMatrix(size_t rows, size_t	columns, int matrix[rows][columns]) {
	for (int i=0 ; i < rows ; i++) {

		for (int j=0 ; j < columns; j++) {
			printf("%d\t", matrix[i][j] );
		}
		printf("\n");
	}
	printf("END OF MATRIX\n" );
}

void initializeMatrix() {

	for (int x = 1 ; x < aR+1; x++) {
		for (int y = 1; y < aC+1; y++) {
			a[x-1][y-1] = x + y;
		}
	}

	for (int x = 1 ; x < bR+1; x++) {
		for (int y = 1; y < bC+1; y++) {
			b[x-1][y-1] = x + (2*y);

		}
	}
}
