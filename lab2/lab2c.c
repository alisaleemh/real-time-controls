#include <stdio.h>
#include <time.h>


double diffTime(double start, double end)
{
  double temp;

  if ((end-start)<0)
  {
    temp = 1000000000+end-start;
  }
  else
  {
    temp = end-start;
  }
  return temp;
}


int main() {

  double maxitr;
  double int1= 0, int2 = 0, result= 0, elapsed_time = 0, time_per_iter = 0;
  char opr;
  struct timespec start_time, end_time;

  // set max iterations
  maxitr = 500000000;

  while(1) {

    // store the inputs
    printf ("Enter integer 1: \n");
    scanf("%lf", &int1);

    printf ("Enter integer 2: \n");
    scanf("%lf", &int2);

    printf("Enter the operation(+, -, *, /):\n");
    scanf("%s", &opr);

    // get start time
    clock_gettime(CLOCK_REALTIME, &start_time);

    // if conditionals for operators
    if (opr == '+') {
      for (int i = 0; i < maxitr ; i++) result = int1 + int2 ;
    }
    else if (opr == '-') {
      for (int i = 0; i < maxitr ; i++) result = int1 - int2 ;
    }
    else if (opr == '*') {
      for (int i = 0; i < maxitr ; i++) result = int1 * int2 ;
    }
    else if (opr == '/') {
      for (int i = 0; i < maxitr ; i++) result = int1 / int2 ;
    }
    else { printf("INVALID OPERATOR, please try again\n\n") ; }

    // get end time
    clock_gettime(CLOCK_REALTIME, &end_time);

    // calculate elapsed_time and time per iteration

    printf("start_time: %lf, end_time: %lf\n", end_time.tv_nsec - start_time.tv_nsec );


    elapsed_time = diffTime(start_time.tv_nsec, end_time.tv_nsec);

    // print elapsed_time and time per iteration
    printf("Total Elapsed Time is: %lf\n", elapsed_time );
    printf("Result is %lf\n", result);

  }


  return(0);
}
