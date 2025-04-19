// need this to compile
#define _POSIX_C_SOURCE 1

#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>


#define TIMEOUT 5

// This is to demo the kill functiona and waitpid with WNOHANG
// This looks very similar to what you should do in penn-shredder
// but you SHOULD NOT do this in penn-shredder. waitpid is not allowed
// for that assignment

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
  pid_t pid = 0;
  int time_passed = 0;
  printf("[parent] My child lives!\n");
  printf("[parent] Waiting for my child to die...\n");

  // check once a second every 5 seconds
  for(time_passed = 0; time_passed < TIMEOUT; time_passed++) {
    printf("[parent] Checking if my child is dead...\n");
    pid = waitpid(cpid, &status, WNOHANG) ;
    if (pid == cpid) {
      printf("[parent] Child is done!!!!!\n");
      break;
    } else if (pid == -1) {
      perror("waitpid error");
      exit(EXIT_FAILURE);
    } else if (pid == 0) {
      printf("[parent] Child is not done...\n");
    }
    sleep(1);
  }

  if (time_passed == TIMEOUT) {
    printf("[parent] My child is taking tooo long!\n");
    kill(cpid, SIGKILL);
  }

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

  printf("[parent] My child has died.\n");
  return EXIT_SUCCESS;
}
