#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>

#define WAITING_TIME 3
pid_t child_pid;

void sighandler(int signum){
	clock_t start = clock();
	while(((double)(clock() - start) / CLOCKS_PER_SEC) < WAITING_TIME){
		if (waitpid(child_pid, NULL, WNOHANG) == child_pid){
			printf("Done!\n");
			exit(EXIT_SUCCESS);
		}
	}
	if (waitpid(child_pid, NULL, WNOHANG) == 0) {
		kill(child_pid, SIGTERM);
	}
}

int main(){
	signal(SIGUSR1, sighandler);

	child_pid = fork();

	if (child_pid == -1) {
		perror("fork failed");
		exit(EXIT_FAILURE);
	}

	if (child_pid == 0) {
		char* argv[] = {"./B", NULL};
        execv(argv[0], argv);

        perror("Something went wrong ...");
		exit(EXIT_FAILURE);
	}

	wait(NULL);
    exit(EXIT_SUCCESS);
}