#include <stdio.h>
#include <rtai_lxrt.h>

int main(void)
{
   RTIME start_time, stop_time, sleep_time;
   RT_TASK *mytask;
   unsigned long my_task_name;
   int i, itr, slept_time[10];;

   // Set this up as a RTAI HARD RT task
   rt_allow_nonroot_hrt();
   my_task_name = nam2num("MYTASK");
   mytask = rt_task_init(my_task_name, 0, 0, 0);

   rt_make_hard_real_time();

   sleep_time = 20000000;

   start_rt_timer(0);

   itr = 10;
   for (i = 0; i < itr; i++) {
      start_time = rt_get_time_ns();
      rt_sleep(nano2count(sleep_time));
      stop_time = rt_get_time_ns();
      slept_time[i] = stop_time-start_time;
   }

   rt_make_soft_real_time();
   rt_task_delete(mytask);

   for (i = 0; i < itr; i++) {
      printf("Iteration %2d ... Slept for %ld nsec\n", i, slept_time[i]);
	}
}
