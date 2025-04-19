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

// bool done = false;
volatile sig_atomic_t done = 0;

void handler(int signo) {
  if (signo == SIGALRM) {
    // printf is not signals safe
    printf("alarm delivered\n");
    done = 1;
  }
}

int main() {
  sigset_t mask;
  sigset_t old_mask;
  
  if (sigemptyset(&mask) == -1) {
    perror("sigemptyset failed, idk how but it did");
    exit(EXIT_FAILURE);
  }

  if(sigaddset(&mask, SIGINT) == -1) {
    perror("sigaddset failed, idk how but it did");
    exit(EXIT_FAILURE);
  }

  // not error checking cause I am too tired
  signal(SIGALRM, handler);
  alarm(5);

  sigsuspend(&mask);


  if (sigprocmask(SIG_UNBLOCK, &mask, &old_mask) == -1) {
    perror("sigprocmask failed, idk how but it did");
    exit(EXIT_FAILURE);
  }

  // could use a while true loop
  while (true) { }

  // could instead empty the block set and call sigsuspend again
  if (sigemptyset(&mask) == -1) {
    perror("sigemptyset failed, idk how but it did");
    exit(EXIT_FAILURE);
  }
  sigsuspend(&mask);
  
  return EXIT_SUCCESS;
}
