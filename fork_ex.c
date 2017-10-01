#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

int global_v; /* A global variable*/

int main(void)
{
    pid_t child_PID;
    int local_v = 0;

    child_PID = fork();

    if(child_PID >= 0) // fork was successful
    {
        if(child_PID == 0) //if youre the child
        {
            local_v++;
            global_v++;
            printf("\n Child Process :: local_v = [%d], global_v[%d]\n", local_v, global_v);
        }
        else //if youre the parent
        {
            local_v = 10;
            global_v = 20;
            printf("\n Parent process :: local_v = [%d], global_v[%d]\n", local_v, global_v);
        }
    }
    else // fork failed
    {
        printf("\n Fork failed, quitting.\n");
        return 1;
    }

    return 0;
}