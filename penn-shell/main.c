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

    int result = parse_command(command, &cmd);
    if (result != 0) {
      fprintf(stderr, "error parsing command\n");
      continue;
    }
    // fork
    pid_t process_id = fork();
    if (process_id < 0) {
      // Bail on failure
      exit(EXIT_FAILURE);
    } else if (process_id == 0) {
      // This is the child

      for (int i = 0; i < cmd->num_commands; i++) {
        fprintf(stderr, "execute command %i: %s\n", i, cmd->commands[0][0]);

        //   int command_items = count_items(cmd->commands[0]);

        // The child runs this command and dies
        execvp(*cmd->commands[i], *cmd->commands);
        fprintf(stderr, "%s \n", "Execvp failed horribly");

        // 1. get_cmd_args(cmd, &cmdArgs)
        // Take in 1 - len(cmd->commands[0])-1
        // 2. Fork and wait?
        // 3. execvp(*cmd->commands[i] + cmdArgs)
      }
    } else {
      // Parent process
      printf("Parent process waiting for child to terminate\n");
      int status;
      // Wait for the child process to terminate
      pid_t child_pid = waitpid(process_id, &status, 0);
      if (child_pid == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
      }
      if (WIFEXITED(status)) {
        printf("Child process exited with status %d\n", WEXITSTATUS(status));
      } else if (WIFSIGNALED(status)) {
        printf("Child process terminated by signal %d\n", WTERMSIG(status));
      }
      printf("Parent process continuing execution\n");
    }

    // This should work...might bite us later???
    // print_parsed_command(stderr, cmd);
    fprintf(stderr, "\n");
    free(cmd);
  }

  return EXIT_SUCCESS;
}