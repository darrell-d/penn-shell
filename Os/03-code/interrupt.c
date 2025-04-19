#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

bool done = false;

void handler(int signo) {
  if (signo == SIGALRM) {
    while(!done) {
      printf("I'm loooooping.....\n");
    }
    printf("I guess I am done... \n");
  }

  if (signo == SIGINT) {
    printf("Got sigint!\n");
    done = true;
  }
}

int main() {
  if (signal(SIGALRM, handler) == SIG_ERR) {
    perror("Can't handle SIGALRM\n");
    exit(EXIT_FAILURE);
  }

  if (signal(SIGINT, handler) == SIG_ERR) {
    perror("Can't handle SIGINT\n");
    exit(EXIT_FAILURE);
  }

  alarm(5);

  while(!done) { }

  return EXIT_SUCCESS;
}

