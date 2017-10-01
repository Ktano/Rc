
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

char* itoa(int val, int base){
    
    static char buf[32] = {0};
    
    int i = 30;
    
    for(; val && i ; --i, val /= base)
        
        buf[i] = "0123456789abcdef"[val % base];
    
    return &buf[i+1];
    
}
