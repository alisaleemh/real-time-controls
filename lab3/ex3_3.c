#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

struct thread_info {
	int maxitr;
	int operation;
	double exec_time;
};

typedef struct thread_info thread_info_t;

void *func(void *arg) {}

void printMatrix(int matrix[][]) {}

int main(void) {

	int a[4][4], b[4][3];

	int aR, bC ;

	// number of rows
	aR = sizeof(a)/sizeof(a[0]);
	bR = sizeof(b)/sizeof(b[0]);

	// number of columns
	aC = sizeof(a[0])/sizeof(a[0][0]);
	bC = sizeof(b[0])/sizeof(b[0][0]);




}
