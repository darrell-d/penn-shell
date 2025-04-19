#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

// this demo is done with another terminal session
// running at the same time occasionally running "ps -u"
// to show the child process going from runnning -> zombie -> gone

int main(int argc, char **argv) {
  printf("[parent] about to fork..\n");
  sleep(8);

  pid_t cpid = fork();
  if (cpid == 0) {
    // child
    printf("[child] I'm alive!\n");
    sleep(10);
    printf("[child] exiting...becoming a zombie.\n");

    // exit the child
    // could also just "return EXIT_SUCCESS;"
    exit(EXIT_SUCCESS);
  } else if (cpid < 0) {
    // parent -- fork failed
    fprintf(stderr,"[parent] fork failed :(\n");
    return EXIT_FAILURE;
  }

  // parent -- fork succeeded
  int status;
  printf("[parent] My child lives!\n");
  sleep(15);

  printf("[parent] Waiting for my child to die...\n");
  wait(&status);

  printf("[parent] My child has died.\n");
  sleep(10);
  return EXIT_SUCCESS;
}
