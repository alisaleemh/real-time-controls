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
float N, Ti, Td;
float P, I, D;
float  sample_freq, step_mag, sq_mag, sq_frequency, sq_duty_cycle;

int no_of_samples, i;

void *Control(void *arg)    {
    int k = 0;
    no_of_samples = (int)(run_time * Fs);
    float T = 1.0/Fs;
    
    float P = 0.0, I = 0.0 , D = 0.0;
    
    while (k < no_of_samples)   {
        sem_wait(&data_avail);
        float motor_position = EtoR(ReadEncoder());
        float ek = 0.0, lek = 0.0;   //Calculate tracking error
        ek = ref[k] - motor_position;
        I = I + Kp/Ti + lek;
        P = Kp * ek;
        D = ((Td/(N*T + Td)) * D ) +( ((Kp*Td*N) / (N*T+Td)) * (ek - lek) );
        float uk = P + I + D;
        DtoA(VtoD(uk));
        theta[k] = motor_position;
        lek = ek;
        k++;
    }
    pthread_exit(NULL);
}

int main(void)  {
    char user_response, input_type[20];
    step_mag = deg;
    int j = 0.0, jmax = 0.0;
    
    N = 20;
    Ti = 0.0001785;
    Td = 0.000044625;
    
    while(1)    {
        
        printf("r: Run the control algorithm \np: Change value of Kp\nf: Change value sample_freq \nt: Change value of run_time \nu: Change the type of inputs (Step or Square) \ng: Plot results on screen \nh: Save the plot results in Postscript \na: Time interval \ni: Change value of Ti \nd: Change value of Td \nn: Change value of N \nq: Exit \n");
        scanf("%s",&user_response);
        
        switch(user_response)   {
            case 'u':
                printf("input: Step or Square \n");
                scanf("%c",&input_type);
                if (strcmp("step", input_type) == 0) {
                    printf("Enter the magnitude of the step response \n");
                    scanf("%lf",&step_mag);
                    for(i = 0; i < MAXS; i++) {
                        ref[i] = (step_mag * (M_PI / 180.0));
                    }
                    
                }
                
                if (strcmp("suqare", input_type) == 0) {
                    printf("Enter the magnitude of the square response \n");
                    scanf("%lf",&sq_mag);
                    printf("Enter the frequency of the square response \n");
                    scanf("%lf",&sq_frequency);
                    printf("Enter the duty cycle of the square response \n");
                    scanf("%lf",&sq_duty_cycle);
                    //Square(y,MAXS,sample_freq,(&magnitude)*pi/180),frequency,duty_cycle);
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
                
            case 'i':
                printf ("Enter the new value of Ti \n");
                scanf ("%f",&Ti);
                break;
                
            case 'd':
                printf ("Enter the new value of Td \n");
                scanf ("%f",&Td);
                break;
                
            case 'n':
                printf ("Enter the new value of N \n");
                scanf ("%f",&N);
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
                
            case 'a':
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
