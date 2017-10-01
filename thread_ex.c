#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>

pthread_t tid[2];

void* work(void *arg)
{


    unsigned long i = 0;
    pthread_t id = pthread_self();
    printf("\n im thread number %lu \n",id); //
    

    for(i=0; i<10;i++){
        printf("b\n");
    }



    return NULL;
}

int main(void)
{
    int i = 0;
    int err;

    while(i < 2)
    {
        err = pthread_create(&(tid[i]), NULL, &work, NULL);
        if (err != 0)
            printf("\nCan't create thread :[%s]", strerror(err));
        else // =0
            printf("\n Thread created.\n");

        i++;
    }

    sleep(5);
    return 0;
}