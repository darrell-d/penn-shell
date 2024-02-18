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

#define READ 0
#define WRITE 1

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

    // make a pipe for child#1
    int pipe1_fds[2];
    if (pipe(pipe1_fds) < 0) {
      perror("pipe error");
      exit(EXIT_FAILURE);
    }

    // fork child#1
    pid_t process_id = fork();
    if (process_id < 0) {
      // Bail on failure
      exit(EXIT_FAILURE);
    } else if (process_id == 0) {
      // This is child #1

      // close the fds we don't need (read)
      close(pipe1_fds[READ]);

      // redirect standard out to write to the pipe
      int ret = dup2(pipe1_fds[WRITE], STDOUT_FILENO);
      if (ret < 0) {
        perror("dup2");
        exit(EXIT_FAILURE);
      }

      // run the first command and die
      execvp(*cmd->commands[0], *cmd->commands);
      fprintf(stderr, "%s \n", "Execvp failed horribly");

      // for (int i = 0; i < cmd->num_commands; i++) {
      //   fprintf(stderr, "execute command %i: %s\n", i, cmd->commands[0][0]);

      //   //   int command_items = count_items(cmd->commands[0]);

      //   // The child runs this command and dies
      //   execvp(*cmd->commands[i], *cmd->commands);
      //   fprintf(stderr, "%s \n", "Execvp failed horribly");

      //   // 1. get_cmd_args(cmd, &cmdArgs)
      //   // Take in 1 - len(cmd->commands[0])-1
      //   // 2. Fork and wait?
      //   // 3. execvp(*cmd->commands[i] + cmdArgs)
      // }
    }

    // now both parent and child#1 should close the write end of the pipe
    close(pipe1_fds[WRITE]);

    if (process_id > 0) {
      // Parent process
      printf("Parent process waiting for child #1 to terminate\n");
      int status;
      // Wait for child process #1 to terminate
      pid_t child_pid = waitpid(process_id, &status, 0);
      if (child_pid == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
      }
      if (WIFEXITED(status)) {
        printf("Child process #1 exited with status %d\n", WEXITSTATUS(status));
      } else if (WIFSIGNALED(status)) {
        printf("Child process #1 terminated by signal %d\n", WTERMSIG(status));
      }
      printf("Parent process continuing execution\n");

      // // close the parent's read end of child#1's pipe
      // close(pipe1_fds[READ]);

      // // make a pipe for child#2
      // int pipe2_fds[2];
      // if (pipe(pipe2_fds) < 0) {
      //   perror("pipe error");
      //   exit(EXIT_FAILURE);
      // }

      // fork child#2
      pid_t process_id2 = fork();
      if (process_id2 < 0) {
        // Bail on failure
        exit(EXIT_FAILURE);
      } else if (process_id2 == 0) {
        // This is child #2

        // child #2 should close its write end of pipe1
        close(pipe1_fds[WRITE]);

        // // child #2 should close its write end of pipe2
        // close(pipe2_fds[WRITE]);

        // redirect standard in to read from the first pipe
        int ret = dup2(pipe1_fds[READ], STDIN_FILENO);
        if (ret < 0) {
          perror("dup2");
          exit(EXIT_FAILURE);
        }

        // If there's a second command, run it using the output from the first command (coming from the parent via pipe2) as input
        if (cmd->num_commands > 1) {
          execvp(*cmd->commands[1], *cmd->commands);
          fprintf(stderr, "%s \n", "Execvp failed horribly");
        } else {
          // If there's no second command, just run `cat` using the output from the first command (coming from the parent via pipe2) as input
          char *argv[] = {"cat", NULL};
          execvp(argv[0], argv);
          fprintf(stderr, "%s \n", "Execvp failed horribly");
        }
      }

      // now both parent and child#2 should close the read end of the first pipe
      close(pipe1_fds[READ]);

      // // now both parent and child#2 should close the read end of the pipe
      // close(pipe2_fds[READ]);

      if (process_id2 > 0) {
        // Parent process
        printf("Parent process waiting for child #2 to terminate\n");
        int status;
        // Wait for child process #2 to terminate
        pid_t child_pid = waitpid(process_id2, &status, 0);
        if (child_pid == -1) {
          perror("waitpid");
          exit(EXIT_FAILURE);
        }
        if (WIFEXITED(status)) {
          printf("Child process #2 exited with status %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
          printf("Child process #2 terminated by signal %d\n", WTERMSIG(status));
        }
        printf("Parent process continuing execution\n");

        // // close the parent's write end of child#2's pipe
        // close(pipe2_fds[WRITE]);
      }
    }

    // This should work...might bite us later???
    // print_parsed_command(stderr, cmd);
    fprintf(stderr, "\n");
    free(cmd);
  }

  return EXIT_SUCCESS;
}