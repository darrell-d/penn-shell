#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

// Demo how to check the status
// after you call wait

int main(int argc, char** argv) {
  int status = 0;
  /* fork another process */
  pid_t pid = fork();

  if (pid < 0) {
    /* error occurred */
    fprintf(stderr, "Fork Failed");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    /* child process */
    char* args[] = {"/bin/sleep", "10", NULL};
    execvp(args[0], args);
  }  else {
    /* parent process */
    /* parent wait for child complete */
    signal(SIGINT, SIG_IGN);
    printf("Creating child %d\n", pid);
    pid_t pid1;

    // check status until it has exited, either normally or via signal
    do {
      pid1 = wait(&status);
      if (pid1 == -1) {
	perror("waitpid");
	exit(EXIT_FAILURE);
      }

      if (WIFEXITED(status)) {
	printf("exited, status=%d\n", WEXITSTATUS(status));
      } else if (WIFSIGNALED(status)) {
	printf("killed by signal %d\n", WTERMSIG(status));
      } else if (WIFSTOPPED(status)) {
	printf("stopped by signal %d\n", WSTOPSIG(status));
      }

    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    
    printf("Child %d complete with status %d\n", pid1, status);
    exit(EXIT_SUCCESS);
  }
}
