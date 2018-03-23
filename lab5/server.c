#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <mqueue.h>
#include <errno.h>

struct Msg 
{
	char str[40]; 
	unsigned int priority;
};

int errno;

const char *client_queue = "/qclient"; 
const char *server_queue = "/qserver";  // Define the name of the MQ

int compute_prime(int n)
{
   int factor, prime, candidate = 2;

   while(1)
   {
      prime = 1;

      for(factor = 2; factor < candidate; ++factor)
      {
         if(candidate % factor == 0)
         {
            prime = 0;
            break;
         }
      }
	  
      if(prime)
      {
         if(--n == 0) //don't use --number because it will decrement "number" by 1 and update it before comparing to 0
         {
            return candidate;
         }
      }
      ++candidate;
   }
   return 0;

}

int main(void *arg)
{
////////////////////////////// Set Queue Properties //////////////////////////////   

   mqd_t client_request_queue, server_return_queue;
   mode_t mode;
   int oflags, nMsgs, result, send_result, val = 0;
   struct mq_attr my_attrs;
   struct Msg myMsg;
   ssize_t n;
   char inBuf[50];
   unsigned int priority;
   myMsg.priority = 1;

   ////////////////////////
   
   my_attrs.mq_maxmsg = 10;      // Set max no of msg in queue to 10 msgs
   my_attrs.mq_msgsize = 50;     // Set the max msg size to 50 bytes

   oflags = O_CREAT | O_RDWR ;   // Set oflags to create queue (O_CREAT)
                                 // and for read & write (O_RDWR)
   
   mode = S_IRUSR | S_IWUSR;     // Set mode to allow other threads
                                 // (created by same user) to use the queue
								 
////////////////////////////// Create the Queues //////////////////////////////

   client_request_queue = mq_open(client_queue, oflags, mode, &my_attrs);  // Create the MQ
   if (client_request_queue < 0) perror("Main mq_open:");  // Error checking

   server_return_queue = mq_open(server_queue, oflags, mode, &my_attrs);  // Create the MQ
   if (server_return_queue < 0) perror("Main mq_open:");  // Error checking
   
////////////////////////////// Get n from client //////////////////////////////
   
   printf("\n---------- Waiting for input ----------\n\n");
   
   while(1)
   {
      mq_getattr(client_request_queue, &my_attrs);     // Determine no. of msgs currently in the MQ
      nMsgs = my_attrs.mq_curmsgs;
	  
	  while(nMsgs > 0)
	  {	  
         n = mq_receive(client_request_queue, inBuf, sizeof(inBuf), &priority); // Grab value from Server Queue
         inBuf[n] = '\0';
         if (n < 0) perror("Receiver mq_receive:");  // Error checking
		 
////////////////////////////// Convert message string to integer //////////////////////////////
		 
		 val = atoi(inBuf);
		 
////////////////////////////// Condition to close queues //////////////////////////////
		 
		 if(val == 0)
	     {
	        mq_close(client_request_queue);   // Close the MQ
			mq_close(server_return_queue);   // Close the MQ
			
			mq_unlink(client_queue);     // Destroy the MQ
			mq_unlink(server_queue);     // Destroy the MQ
			
            pthread_exit(NULL);
			exit(0);
		 }

////////////////////////////// Compute //////////////////////////////
		 
		 result = compute_prime(val);
		 
		 printf("Number sent by client = %d\n", val);
		 printf("No. %d prime number = %d\n", val, result);		 
		 printf("\n---------- Waiting for input ----------\n\n");
		 
////////////////////////////// Convert integer to string //////////////////////////////
         
		 sprintf(myMsg.str, "The No. %d prime number is: %d", val, result);
		 
////////////////////////////// Send to Client //////////////////////////////
		 
		 send_result = mq_send(server_return_queue, myMsg.str, (size_t)strlen(myMsg.str), myMsg.priority);
		 if (send_result < 0) perror("Sender mq_send:");  // Error checking

		 mq_getattr(client_request_queue, &my_attrs);     // Update nMsgs
         nMsgs = my_attrs.mq_curmsgs;
	  }
   }

   exit(0);
}
