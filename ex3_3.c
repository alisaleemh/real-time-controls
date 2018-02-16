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

void *func(void *arg) {
        struct timespec time_1, time_2;
        double exec_time;
        thread_info_t *info;

 int i, maxitr, operation;
 volatile double a, b, c;

 info = (thread_info_t *)arg;
 maxitr = info->maxitr;
 operation = info->operation;
 b = 2.3; c = 4.5;

 exec_time = 0.0;

 switch (operation){
         case 1:
                 clock_gettime(CLOCK_REALTIME, &time_1);
                 for (i = 0; i < maxitr ; i++) {
                         a = b + c;
                 }
         break;
         case 2:
                 clock_gettime(CLOCK_REALTIME, &time_1);
                 for (i = 0; i < maxitr ; i++) {
                         a = b - c;
                 }
         break;
         case 3:
                 clock_gettime(CLOCK_REALTIME, &time_1);
                 for (i = 0; i < maxitr ; i++) {
                         a = b * c;
                 }
         break;
         case 4:
                 clock_gettime(CLOCK_REALTIME, &time_1);
                 for (i = 0; i < maxitr ; i++) {
                         a = b / c;
                 }
         break;
 }
 clock_gettime(CLOCK_REALTIME, &time_2);

 exec_time = (time_2.tv_sec - time_1.tv_sec);
 exec_time = exec_time + (time_2.tv_nsec - time_1.tv_nsec)/1.0e9;

 info->exec_time = exec_time;
 pthread_exit(NULL);
}

int main(void) {

        pthread_t threads[4];
        thread_info_t infos[4];

        double maxitr = 5.0e8;

        int i;
        for (i = 0; i < 4; i++) {
                infos[i].maxitr = (int)maxitr;
                infos[i].operation = i+1;
                pthread_create(&threads[i], NULL, &func, &infos[i]);
        }

        pthread_join(threads[0], NULL);
        printf("ADD = %lf sec\n", infos[0].exec_time);

        pthread_join(threads[1], NULL);
        printf("SUB = %lf sec\n", infos[1].exec_time);

        pthread_join(threads[2], NULL);
        printf("MULT = %lf sec\n", infos[2].exec_time);

        pthread_join(threads[3], NULL);
        printf("DIV = %lf sec\n", infos[3].exec_time);

        pthread_exit(NULL);
}
