#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE 1
#endif

#define BUF_LEN 4096U
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "helpers.h"
#include "parser.h"

int main() {
  // putc('\0', stdin);
  setup_sig_handlers();

  // setup_sig_handlers();
  while (true) {
    char command[BUF_LEN] = {0};
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
      // EOF recieved with no input. Exit
      break;
    }

    command[res] = '\0';
    // If no new line print one out. Part of handling EOF with input
    if (command[res - 1 != '\n']) {
      fprintf(stderr, "\n");
    }

    struct parsed_command* cmd = NULL;

    int result = parse_command(command, &cmd);
    if (result != 0) {
      fprintf(stderr, "error parsing command\n");
      continue;
    }

    // Initialize a pipe holder
    struct pipe_holder* holder = NULL;

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

void setup_sig_handlers() {
  if (signal(SIGINT, signal_silencer) == SIG_ERR) {
    printf("Can't handle SIGINT\n");
    exit(EXIT_FAILURE);
  }
  if (signal(SIGTTIN, signal_silencer) == SIG_ERR) {
    printf("Can't handle SIGTTIN\n");
    exit(EXIT_FAILURE);
  }
  if (signal(SIGTTOU, signal_silencer) == SIG_ERR) {
    printf("Can't handle SIGTTOU\n");
    exit(EXIT_FAILURE);
  }
  if (signal(SIGQUIT, signal_silencer) == SIG_ERR) {
    printf("Can't handle SIGQUIT\n");
    exit(EXIT_FAILURE);
  }
  if (signal(SIGTSTP, signal_silencer) == SIG_ERR) {
    printf("Can't handle SIGTSTP\n");
    exit(EXIT_FAILURE);
  }
}