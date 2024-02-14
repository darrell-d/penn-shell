#define BUF_LEN 4096U
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "parser.h"

#ifndef PROMPT
#define PROMPT "penn-shredder# "
#endif

int main() {
  char command[BUF_LEN] = {0};

  while (true) {
    ssize_t res = write(STDERR_FILENO, PROMPT, strlen(PROMPT));
    if (res < 0) {
      fprintf(stderr, "error prompting user\n");
      exit(EXIT_FAILURE);
    }

    res = read(STDIN_FILENO, command, BUF_LEN - 1);
    if (res < 0) {
      fprintf(stderr, "error prompting user\n");
      exit(EXIT_FAILURE);
    } else if (res == 0) {
      break;
    }

    command[res] = '\0';

    struct parsed_command* cmd = NULL;
    int result = parse_command(command, &cmd);
    if (result != 0) {
      fprintf(stderr, "error parsing command\n");
      continue;
    }

    print_parsed_command(stderr, cmd);
    fprintf(stderr, "\n");
    free(cmd);
  }

  return EXIT_SUCCESS;
}