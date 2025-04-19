#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int main() {
  int status = 0;
  /* fork another process */
  pid_t pid = fork();

  if (pid < 0) {
    /* error occurred */
    fprintf(stderr, "Fork Failed");
    return EXIT_FAILURE;
  } else if (pid == 0) {
    /* child process */
    char* args[] = {"/bin/sleep", "10", NULL};
    execvp(args[0], args);
    return EXIT_FAILURE;
  }  else {
    pid_t res = waitpid(pid, &status, 0);
    while (res == 0) {
      // no status update yes
      printf("waiting...\n");
      res = waitpid(pid, &status, WNOHANG);
    }
    printf("Done!\n");
    return EXIT_SUCCESS;
  }
}
