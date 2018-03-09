#include <stdio.h>       /* for fprintf() */
#include <stdlib.h>      /* for exit() */
#include <unistd.h>      /* for sleep() */
#include <pthread.h>     /* for pthreads functions */

#define NTHREADS 2

int string_index = 0;

char string_to_print[] = "0123456789";

pthread_mutex_t index_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t index_cv = PTHREAD_COND_INITIALIZER;

void* func1(void* arg)
{

    while(1) {

    pthread_mutex_lock(&index_mutex);

    while (string_index > 4)
    {
        pthread_cond_wait(&index_cv, &index_mutex);
    }

    while (string_index <= 4)
    {
        printf("A%c\t", string_to_print[string_index] );
        string_index++;
    }

    if (string_index > 10)
    {
        string_index = 0;
    }

    if (string_index >= 4)
    {
        pthread_cond_broadcast(&index_cv);
    }



    pthread_mutex_unlock(&index_mutex);

}

}

void* func2(void* arg)
{

    while(1)
    {

    pthread_mutex_lock(&index_mutex);



    while (string_index <= 9)
    {
        printf("B%c\t", string_to_print[string_index] );
        string_index++;
    }


    if (string_index >= 9)
    {
        string_index = 0;
        printf("\n");

    }

    if (string_index < 4)
    {
        pthread_cond_broadcast(&index_cv);
    }

    while (string_index < 4)
    {
        pthread_cond_wait(&index_cv, &index_mutex);
    }


    pthread_mutex_unlock(&index_mutex);

}

}


int main(void)
{
    pthread_t threads[NTHREADS];
    int k;

    pthread_create(&threads[0], NULL, &func1, NULL) ;
    pthread_create(&threads[1], NULL, &func2, NULL);



    sleep(15);   /* sleep 20 secs to allow time for the threads to run */
    /* before terminating them with pthread_cancel()      */

    for (k = 0; k < NTHREADS; k++) {
        pthread_cancel(threads[k]);
    }

    pthread_exit(NULL);
}
