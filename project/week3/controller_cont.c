#include "dlab.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <semaphore.h>

#define MAXS 5000
#define MOTOR_NUM 5
#define ROTATION_D 55.0    //Degrees

#define ERR_INPUT 0

void printHelp();
void printError();
void *Control(void *args);
float satblk(float v);

float theta[MAXS];
float ref[MAXS] = {[0 ... (MAXS - 1)] = (ROTATION_D * (M_PI / 180.0))};

float Kp = 25.8;
float Ti = 0.025;
float Td = 0.00625;
float Tt = 0.01;
float N = 20;
float Fs = 200.0;
float run_time = 3.0;

float Kp_buf;
float Ti_buf;
float Td_buf;
float N_buf;
float Fs_buf;

int stop_flag = 0;

char command;

pthread_t control_thread;
sem_t data_avail;

int main(void *args) {

  char input[256];
  float step_mag = ROTATION_D;
  float square_mag = 0;
  float square_freq = 0;
  float square_dc = 0;
  int i;

  Kp_buf = Kp;
  Ti_buf = Ti;
  Td_buf = Td;
  N_buf = N;
  Fs_buf = Fs;

  printf("Enter 'H' for a list of commands\n");
  while(1) {

    printf("Enter Command: ");
    scanf(" %c", &command);

    switch(command) {
      default:
      printError(ERR_INPUT);
      break;
      case 'r':
      sem_init(&data_avail, 0, 0);
      Initialize(DLAB_SIMULATE, Fs, MOTOR_NUM);
      pthread_attr_t attr;
      pthread_attr_init(&attr);
      pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
      pthread_create(&control_thread, &attr, &Control, NULL);
      pthread_attr_destroy(&attr);
      //pthread_join(control_thread, NULL);
      // Terminate();
      // sem_destroy(&data_avail);
      break;
      case 'p':
      printf("Enter new Kp: ");
      scanf("%f", &Kp_buf);
      break;
      case 'f':
      printf("Enter new sample_freq: ");
      scanf("%f", &Fs_buf);
      break;
      case 't':
      printf("Enter new run_time: ");
      scanf("%f", &run_time);
      break;
      case 'u':
      printf("Select type of input ('step' or 'square'): ");
      scanf("%s", &input);
      if (strcmp("step", input) == 0) {
        printf("Enter magnitude of step in degrees: ");
        scanf("%f", &step_mag);
        for(i = 0; i < MAXS; i++) {
          ref[i] = (step_mag * (M_PI / 180.0));
        }
      } else if (strcmp("square", input) == 0) {
        printf("Enter magniture in degrees: ");
        scanf("%f", &square_mag);
        printf("Enter frequency: ");
        scanf("%f", &square_freq);
        printf("Enter duty cycle: ");
        scanf("%f", &square_dc);
        Square(ref, MAXS, Fs, (square_mag * (M_PI / 180.0)), square_freq, square_dc);
      } else {
        printError(ERR_INPUT);
      }
      break;
      case 'g':
      plot(ref, theta, Fs, (int) (run_time * Fs), SCREEN, "Response", "Time [s]", "Magnitude [rad]");
      break;
      case 'h':
      plot(ref, theta, Fs, (int) (run_time * Fs), PS, "Response", "Time [s]", "Magnitude [rad]");
      break;
      case 'q':
      exit(0);
      break;
      case 'H':
      printHelp();
      break;
      case 's':
      for(i = 400; i < 600; i++) {
        printf("%.3fs\t%f rad\n", (i/200.0), theta[i]);
      }
      break;
      case 'i':
      printf("Enter new Ti: ");
      scanf("%f", &Ti_buf);
      break;
      case 'd':
      printf("Enter new Td: ");
      scanf("%f", &Td_buf);
      break;
      case 'n':
      printf("Enter new N: ");
      scanf("%f", &N_buf);
      break;
      case 'x':
      stop_flag = 1;
      break;
    }

  }
}

void *Control(void *args) {
  int k;
  float p, i, d = 0;
  float error, error_past = 0;
  float motor_pos, control_val, t_samp = 0;
  float i_past, d_past = 0;
  float anti_windup = 0;
  t_samp = 1 / Fs;
  while(stop_flag == 0) {
    Kp = Kp_buf;
    Ti = Ti_buf;
    Td = Td_buf;
    N = N_buf;
    Fs = Fs_buf;
    for (k = 0; k < ((int) (run_time * Fs)); k++) {
      sem_wait(&data_avail);
      motor_pos = EtoR(ReadEncoder());
      error = ref[k] - motor_pos;
      p = Kp * error;
      i = i_past + (Kp / Ti) * error_past * t_samp + anti_windup * (1 / Tt) * t_samp;
      d = ((Td / (N * t_samp) + Td) * d_past) +
      ((Kp * Td * N) / ((N * t_samp) + Td) * (error - error_past));
      float v = p + i + d;
      control_val = satblk(v);
      anti_windup = control_val - v;
      DtoA(VtoD(control_val));
      theta[k] = motor_pos;
      error_past = error;
      i_past =  i;
      d_past = d;
    }
  }
  stop_flag = 0;
  Terminate();
  sem_destroy(&data_avail);
  pthread_exit(NULL);
}

float satblk(float v) {
  if(v > 1.4) {
    v = 1.4;
  } else if (v < -1.4) {
    v = -1.4;
  }
  return v;
}

void printError(int err) {
  switch(err) {
    case ERR_INPUT:
    printf("Invalid entry!\n\n");
    break;
  }
}

void printHelp() {
  printf("Available commands:\n");
  printf("r: Run the control algorithm\n");
  printf("p: Change the value of Kp\n");
  printf("f: Change the value of sample_freq\n");
  printf("t: Change the value of run_time\n");
  printf("u: Change the type of inputs (Step or Square)\n");
  printf("g: Plot results on screen\n");
  printf("h: Save the Plot results in Postscript\n");
  printf("s: Print time vs amplitude for the previous simulation\n");
  printf("i: Change the value of Ti\n");
  printf("d: Change the value of Td\n");
  printf("n: Change the value of N\n");
  printf("x: Stop Program Execution\n");
  printf("q: exit\n");
}
