#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void handler(int signo) {

}


int main() {
  alarm(2);

  return EXIT_SUCCESS;
}

