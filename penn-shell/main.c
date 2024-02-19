#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE 1
#endif

#define BUF_LEN 4096U
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "helpers.h"
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

    // Initialize a pipe holder
    struct pipe_holder *holder = NULL;

    // If there are commands, generate pipes and execute them
    if (cmd->num_commands > 0) {
      holder = create_pipe_holder(cmd->num_commands - 1);
      for (int i = 0; i < cmd->num_commands; i++) {
        execute_command(cmd, holder, i);
      }
    }

    fprintf(stderr, "\n");
    free(cmd);
    free(holder);
  }

  return EXIT_SUCCESS;
}