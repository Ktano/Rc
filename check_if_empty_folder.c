#include <stdio.h>
#include <stdlib.h>



int check_if_empty_folder(char* folder);

int main(int argc, char *argv[]) {



    if(check_if_empty_folder("input_files"))
        printf("empty\n");
    else
        printf("not empty\n");

    return 0;
}


int check_if_empty_folder(char* folder){

    char cmd_for_pc[1024];
  
    int status, exit_status;

  

    snprintf(cmd_for_pc, 1024, "test $(ls -A \"%s\" 2>/dev/null | wc -l) -ne 0", folder);

    status = system(cmd_for_pc);
    exit_status = WEXITSTATUS(status);


    if (exit_status == 1)
            return 1;
    else
            return 0;

}