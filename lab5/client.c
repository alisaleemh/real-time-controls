#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
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
int stop_printing = 0;
int wait_to_print = 0;

const char *client_queue = "/qclient"; 
const char *server_queue = "/qserver";  // Define the name of the MQ
   
void *client_prompt(void *arg)  // This thread sends msgs to the MQ
{
   int send_n, n = 1; // make sure n != 0 at first, otherwise in might run the "if(n == 0)" statement prematurely
   mqd_t client_request_queue, server_return_queue;
   struct Msg myMsg;
   char inBuf[50];
   char *chk;

   sleep(2);
   
////////////////////////////// Open MQ so it is local to this function //////////////////////////////

   client_request_queue = mq_open(client_queue, O_WRONLY);  // Open the MQ for writing only (O_WRONLY)
   if (client_request_queue < 0) perror("Sender mq_open:");  // Error checking
   
////////////////////////////// Grab integer from user ////////////////////////////// 

   while(1)
   {	  	   
	  if(wait_to_print == 0) // don't ask user again for input until prime number from previous task has been calculated
	  {	

////////////////////////////// Prevent user from inputting non-integers //////////////////////////////
         do
		 {
            printf("Enter nth prime number to compute:  ");
            fflush(stdout);
		    if(fgets(inBuf, sizeof(inBuf), stdin) != NULL)
		    {
               n = (int) strtol(inBuf, &chk, 10); /* assume decimal format */
            }
			printf("\n");
			
		 }while(!isspace(*chk) && *chk != 0);

//////////////////////////////////////////////////////////////////////////////////////////////////////
         sprintf(myMsg.str, "%d", n);  // convert integer to string
	  
         myMsg.priority = 1;    // Set up priority of this message
	  
         send_n = mq_send(client_request_queue, myMsg.str, (size_t)strlen(myMsg.str), myMsg.priority); //send message to message queue	    
         if (send_n < 0) perror("Sender mq_send:");  // Error checking

         wait_to_print = 1;	// block it from getting inside if statement	 
	  }
	  
	  if(n == 0)
	  {
		 mq_close(client_request_queue);   // Close the MQ
		 stop_printing = 1;
         pthread_exit(NULL);
	  }
   }
}

void *client_print(void *arg)  // This thread receives msgs from the MQ
{
   mqd_t server_return_queue;
   struct mq_attr my_attrs;
   ssize_t n;
   int nMsgs;
   char inBuf[50];
   unsigned int priority;

   sleep(2);

////////////////////////////// Open MQ so it is local to this function //////////////////////////////  

   server_return_queue = mq_open(server_queue, O_RDONLY);  // Open the MQ for reading only (O_RDONLY)
   if (server_return_queue < 0 ) perror("Receiver mq_open:");  // Error checking

////////////////////////////// Print calculated prime number ////////////////////////////// 

   while(1) 
   {
	  mq_getattr(server_return_queue, &my_attrs);     // Determine no. of msgs currently in the MQ
      nMsgs = my_attrs.mq_curmsgs; 
	   
	  while(nMsgs > 0)
	  {	  
         n = mq_receive(server_return_queue, inBuf, sizeof(inBuf), &priority); // Grab value from Server Queue
         inBuf[n] = '\0';
         if (n < 0) perror("Receiver mq_receive:");  // Error checking
		 
         printf("%s\n", inBuf); // print out message from server
		 printf("\n----------------------------------\n\n");
		 
		 wait_to_print = 0; // allow other thread to ask for user input again
		 
		 mq_getattr(server_return_queue, &my_attrs);     // Update nMsgs
         nMsgs = my_attrs.mq_curmsgs;
	  }
	  
	  if(stop_printing == 1)
	  {
	     mq_close(server_return_queue);   // Close the MQ
         pthread_exit(NULL);
	  }
   }
}

int main(void *arg)
{  
////////////////////////////// Create Threads //////////////////////////////

   pthread_t client_prompt_thread, client_print_thread;

   if (pthread_create(&client_prompt_thread, NULL, &client_prompt, NULL) != 0) 
   {
       printf("Error creating Sender thread.\n");
       exit(-1);
   }

   if (pthread_create(&client_print_thread, NULL, &client_print, NULL) != 0) 
   {
       printf("Error creating Receiver thread.\n");
       exit(-1);
   }
   
   printf("\n---------- Enter 0 at any time to exit ----------\n\n");

   pthread_join(client_prompt_thread, NULL);
   pthread_join(client_print_thread, NULL);
   
////////////////////////////// Close stuff //////////////////////////////

   pthread_exit(NULL);
   exit(0);
}
