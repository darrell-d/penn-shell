#define BUF_LEN 4096U
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "parser.h"

#ifndef PROMPT
#define PROMPT "penn-shredder# "
#endif

// 1. Run and wait for input from std in X
// Block and wait for input from stdin X
// while not sig kill (or other ending sequence)
// Listen for input
// Recieve input
//                  2. Parse out the commands (just a function call)
// 3. Run the commands
// execev
// start by executing ls command
// run the cat command with input
// .. other things
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

    // struct parsed_command {
    //   .is_background = true,
    //   .num_commands = 5,
    //   .commands = {
    //     { "cat", "mutual_aid.txt", "basic.ics", "means_and_ends.pdf", NULL }
    //     // index 0 { "grep", "comrade", NULL } // index 1 { "wc", "-l, "-c",
    //     "NULL" } // index 2 { "cat", "NULL" } // index 3 { "sleep", "10",
    //     "NULL" } // index 4
    //   }
    // }
    int result = parse_command(command, &cmd);
    if (result != 0) {
      fprintf(stderr, "error parsing command\n");
      continue;
    }

    // Pull out into function
    for (int i = 0; i < cmd->num_commands; i++) {
      fprintf(stderr, "execute command %i: %s\n", i, cmd->commands[0][0]);
      // 1. get_cmd_args(cmd, &cmdArgs)
      // 2. Fork and wait?
      // 3. execvp(*cmd->commands[i] + cmdArgs)
    }

    // This should work...might bite us later???
    // print_parsed_command(stderr, cmd);
    fprintf(stderr, "\n");
    free(cmd);
  }

  return EXIT_SUCCESS;
}