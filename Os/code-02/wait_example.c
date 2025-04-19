#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

// this demo is done with another terminal session
// running at the same time occasionally running "ps -u"
// to show the child process going from runnning -> zombie -> gone

int main(int argc, char **argv) {
  printf("[parent] about to fork..\n");

  pid_t cpid = fork();
  if (cpid == 0) {
    // child
    printf("[child] I'm alive!\n");

    // exit the child
    // could also just "return EXIT_SUCCESS;"
    char* args[] = {"/bin/sleep", "10", NULL};
    char* envp[] = { NULL };
    execve(args[0], args, envp);

    exit(EXIT_FAILURE);
  } else if (cpid < 0) {
    // parent -- fork failed
    perror("fork failed :(\n");
    return EXIT_FAILURE;
  }

  // parent -- fork succeeded
  int status;
  printf("[parent] My child lives!\n");
  printf("[parent] Waiting for my child to die...\n");

  // we assume that the process exits either
  // - normally at end of /bin/sleep 10 execution
  // - error with exec
  //
  // you should use something like WIFEXITED(status)
  pid_t wpid = wait(&status);
  if (wpid < 0) {
    perror("wait error");
    return EXIT_FAILURE;
  }

  // NOTE: this print statement isn't printeed till the chlid has finished!
  // if exec worked, then that is at least 10 seconds
  printf("[parent] My child has died.\n");
  return EXIT_SUCCESS;
}
