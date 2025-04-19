#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE 1
#endif

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

bool done = false;

void handler(int signo) {
  if (signo == SIGALRM) {
    printf("alarm delivered\n");
    done = true;
  }
  if (signo == SIGINT) {
    printf("got sigint\n");
  }
}

int main() {
  sigset_t mask;
  sigset_t old_mask;
  
  // initialize the set
  if (sigemptyset(&mask) == -1) {
    perror("sigemptyset failed, idk how but it did");
    exit(EXIT_FAILURE);
  }

  // add SIGINT to the set
  if(sigaddset(&mask, SIGINT) == -1) {
    perror("sigaddset failed, idk how but it did");
    exit(EXIT_FAILURE);
  }

  // block SIGINT
  if (sigprocmask(SIG_BLOCK, &mask, &old_mask) == -1) {
    perror("sigprocmask failed, idk how but it did");
    exit(EXIT_FAILURE);
  }

  // not error checking cause I am too tired
  signal(SIGALRM, handler);
  signal(SIGINT, handler);
  alarm(5);

  while (!done) { }

  // after alarm, unblock sigint
  if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1) {
    perror("sigprocmask failed, idk how but it did");
    exit(EXIT_FAILURE);
  }

  // infinitely loop, SIGINT should now be able
  // to terminate us
  while (true) { }
  
  return EXIT_SUCCESS;
}
