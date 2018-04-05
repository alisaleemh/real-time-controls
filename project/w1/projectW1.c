//
//  main.c
//  ProjectA
//
//  Created by Anirban Mandal on 2017-03-19.
//  Copyright © 2017 Anirban Mandal. All rights reserved.
//
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <sys/mman.h>
#include <pthread.h>
#include <malloc.h>
#include <stdlib.h>
#include "dlab.h"


pthread_t Cont;
sem_t data_avail;

#define MAXS 5000
#define motor_number 2  //Change as required
#define deg 55.0


float theta[MAXS];
float ref[MAXS] = {[0 ... (MAXS - 1)] = (deg * (M_PI / 180.0))};

float Kp = 1.0, run_time = 3.0, Fs = 200.0;
float step_mag, sq_mag, sq_frequency, sq_duty_cycle;
int no_of_samples, i;

void *Control(void *arg)    {
    int k = 0;
    no_of_samples = (int)(run_time * Fs);
    while (k < no_of_samples)   {
        sem_wait(&data_avail);
        float motor_position = EtoR(ReadEncoder());
        float ek = ref[k] - motor_position;   //Calculate tracking error
        float uk = Kp * ek;
        DtoA(VtoD(uk));
        theta[k] = motor_position;
        k++;
    }
    pthread_exit(NULL);
}

int main(void)  {
    char user_response,input_type;
    step_mag = deg;
    int f = 1;
    int j = 0.0, jmax = 0.0;
    int sample_freq;

    while(1)    {

        printf("r: Run the control algorithm \np: Change value of Kp\nf: Change value sample_freq \nt: Change value of run_time \nu: Change the type of inputs (Step or Square) \ng: Plot results on screen \nh: Save the plot results in Postscript \na: Time interval \nq: Exit \n");
        scanf("%s",&user_response);

        switch(user_response)   {
            case 'u':
                printf("input: Step (b) or Square (v) \n");
                while(f){

                    scanf("%c",&input_type);
                    switch(input_type){
                        case 'b':
                            printf("Enter the magnitude of the step response \n");
                            scanf("%lf",&step_mag);
                            for(i = 0; i < MAXS; i++) {
                                ref[i] = (step_mag * (M_PI / 180.0));

                            }
                            f=0;
                            break;
                        case 'v':
                            printf("Enter the magnitude of the square response \n");
                            scanf("%lf",&sq_mag);
                            printf("Enter the frequency of the square response \n");
                            scanf("%lf",&sq_frequency);
                            printf("Enter the duty cycle of the square response \n");
                            scanf("%lf",&sq_duty_cycle);
                            Square(ref,MAXS,Fs,sq_mag*PI/180.0,sq_frequency,sq_duty_cycle);
                            f=0;
                            break;
                    }
                }
                break;

            case 'r':
                sem_init (&data_avail,0,0);
                Initialize(DLAB_SIMULATE, Fs, motor_number);
                pthread_create (&Cont,NULL,&Control,NULL);
                pthread_join(Cont,NULL);
                Terminate();
                sem_destroy (&data_avail);
		break;

            case 'p':
                printf ("Enter the new value of Kp \n");
                scanf ("%f",&Kp);
                break;

            case 'f':
                printf ("Enter the new valve of sample_freq \n");
                scanf ("%f",&sample_freq);
                break;

            case 't':
                printf ("Enter the new value of run_time \n");
                scanf ("%f",&run_time);
                break;

            case 'g':
                printf ("Plotting the graph on Screen \n");
                plot(ref, theta, Fs, no_of_samples, SCREEN, "Respone", "Time (s)", "Magnitude");
                break;

            case 'h':
                printf ("Saving the Plot result in Postscript \n");
                plot(ref, theta, Fs, no_of_samples, PS, "Respone", "Time (s)", "Magnitude");
                break;

            case 'q':
                printf ("We are Done \n");
                exit(0);
	case'a':
	  jmax = run_time * 200.0;
	  for(j = 1; j <= jmax; j++)
	    printf("%lf \t \t %lf rad\n",(j/200.0), theta[j]);
		   break;

            default:
                printf("Error, Invalid Selection \n");
                break;
        }
    }
}
