#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void handler_shmandler(int signo) {
  if (signo == SIGINT) {
    printf("You can't kill me! I am INVINCIBLE >:]\n");
  }
}

int main() {
  if (signal(SIGINT, handler_shmandler) == SIG_ERR) {
    printf("Can't handle SIGINT :(\n");
    exit(EXIT_FAILURE);
  }

  while(true);  // infinitely loop

  // need to use the terminal command `kill` to kill this process
  // use `ps -u` to determine the process id

  return EXIT_SUCCESS;
}

