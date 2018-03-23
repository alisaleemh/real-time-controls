#include <stdio.h>
#include <pthread.h>
#include <mqueue.h>


int computePrime(int num)
{
    int i;
    int prime = 2;

    while(1)
    {
        int isPrime = 1;
        for(i = 2 ; i < prime ; i++)
        {
            if (prime % i == 0)
            {
                isPrime = 0;
            }
    
        }
        if(isPrime == 1) {
            num--;
        }
	if(num == 0)  return prime ;
	prime++;
    }
    
    return prime;

}

int main(int argc, char const *argv[]) {
    int num;
    printf("Enter number: \n");
    scanf("%d", &num);
    int result = computePrime(num);
    printf("Prime value %d\n", result);
    return 0;
}
