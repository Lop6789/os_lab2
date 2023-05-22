#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

// void print_arr(char** argv, int argc){
//     for (int i = 0; i < argc; i++){
//         printf("%s\n", argv[i]);
//     }
// }

int cmd_count(char* str){
    if (!str) return 0;

    int i = 0;
    while (*str){
        if (*str == '|') i++;
        str++;
    }
    return i+1;
}

char** append_elem (char** str_arr, char* str, int cur_size){
    if (!cur_size) str_arr = malloc(sizeof(char*));
    else str_arr = reallocarray(str_arr, cur_size+1, sizeof(char*));
    str_arr[cur_size]  = str;
    return str_arr;
}


int main(){

    // printf("EXEC DONE\n");
    //readline var causes valgrind still reachable

    char* inp = NULL;
    while (!inp){
        inp = readline("Enter arguments: ");
    }

    //For test:
    // char* test = "ls -l|grep B|cat";
    // char* inp = strdup(test);
    
   
    pid_t ppid = getppid();
    kill(ppid, SIGUSR1);

   //FILE DESCRIPTORS
    int fd1[2] = {-1, -1};
    int fd2[2];

    close(fd1[0]);
    close(fd1[1]);

    int cmd_qty = cmd_count(inp);

    char* str_progs = strtok(inp, "|");

    for (int i = 0; i < cmd_qty; i++){
        if (i != 0){
            str_progs = strtok(NULL, "|");
            fd1[0] = fd2[0];
            fd1[1] = fd2[1];
        }

        pipe(fd2);
        pid_t pid= fork();

        if (!pid){

            //prev           
            close(fd1[1]);
            //first 
            if (i != 0) {
                dup2(fd1[0], STDIN_FILENO);
            }

            //next
            close(fd2[0]);
            //last
            if (i != cmd_qty - 1){
                dup2(fd2[1], STDOUT_FILENO);
            }

            char** argv = NULL;
            int argc = 0;
            char* str_args = strtok(str_progs, " ");
            while(str_args){
                argv = append_elem(argv, str_args, argc);
                argc++;
                str_args = strtok(NULL, " ");
            }
            argv = append_elem(argv, NULL, argc);
            argc++;

            execvp(argv[0], argv);

            perror("Something went wrong ...");
		    exit(EXIT_FAILURE);
        }

        close(fd1[0]);
        close(fd2[1]);

        wait(NULL);
    }

    free(inp);
    exit(EXIT_SUCCESS);
}