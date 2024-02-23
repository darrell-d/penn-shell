#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE 1
#endif

#include "helpers.h"
#include <unistd.h>

// Dynamically allocate a pipe_holder struct
struct pipe_holder* create_pipe_holder(size_t num_pipes) {
  struct pipe_holder* holder =
      malloc(sizeof(struct pipe_holder) + num_pipes * sizeof(size_t[2]));
  if (holder == NULL) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }
  holder->num_pipes = num_pipes;
  return holder;
}

// Wait for all children to terminate
void wait_for_children(struct parsed_command* cmd) {
  for (int i = 0; i < cmd->num_commands; i++) {
    int status;
    pid_t child_pid = -1;
    if (cmd->is_background) {
      child_pid = waitpid(-1, &status, WNOHANG);
      // Re-prompt user
      prompt_user();

    } else {
      fprintf(stderr, "not bg\n");
      child_pid = waitpid(-1, &status, 0);
    }

    if (child_pid == -1) {
      perror("waitpid");
      exit(EXIT_FAILURE);
    }
  }
}

void signal_silencer(int signo) {
  if (signo == SIGINT || signo == SIGTTIN || signo == SIGTTOU ||
      signo == SIGQUIT || signo == SIGTSTP) {
    // Print newline and igore signal
    fprintf(stderr, "\n");
    // Re-prompt user
    prompt_user();
  }
}
void signal_silencer_child(int signo) {
  if (signo == SIGINT || signo == SIGTTIN || signo == SIGTTOU ||
      signo == SIGQUIT || signo == SIGTSTP) {
    // Print newline and igore signal
    fprintf(stderr, "\n child handler\n");
    // Re-prompt user
    prompt_user();
  }
}
void child_bg_sig_handler(int signo) {
  if (signo == SIGCHLD) {
    // Print newline and igore signal
    fprintf(stderr, "\n got a signal back from child!");
    // Re-prompt user
    prompt_user();
  }
}

void prompt_user() {
  ssize_t res = write(STDERR_FILENO, PROMPT, strlen(PROMPT));
  if (res < 0) {
    fprintf(stderr, "error prompting user\n");
    exit(EXIT_FAILURE);
  }
}

// Execute all commands in parallel
void execute_commands(struct parsed_command* cmd, struct pipe_holder* holder) {
  // Fork all necessary children and create all pipes at the start
  for (int i = 0; i < cmd->num_commands; i++) {
    // n - 1 pipes for n commands
    if (i < cmd->num_commands - 1) {
      // Make a pipe
      int pipe_fds[2];
      if (pipe(pipe_fds) < 0) {
        perror("pipe error");
        exit(EXIT_FAILURE);
      }
      // Add to pipe holder
      holder->pipes[i][0] = pipe_fds[0];
      holder->pipes[i][1] = pipe_fds[1];
    }

    // Fork a child
    pid_t process_id = fork();
    if (process_id < 0) {
      perror("fork");
      exit(EXIT_FAILURE);
    } else if (process_id == 0) {
      // if (signal(SIGTSTP, signal_silencer_child) == SIG_ERR) {
      //   printf("Can't handle SIGTSTP\n");
      //   exit(EXIT_FAILURE);
      // }
      // Child process

      if (i > 0) {
        // Not the first pipe
        // Close the write end of the previous pipe
        close(holder->pipes[i - 1][WRITE]);

        // Redirect stdin to read from the previous pipe
        int ret = dup2(holder->pipes[i - 1][READ], STDIN_FILENO);
        if (ret < 0) {
          perror("dup2");
          exit(EXIT_FAILURE);
        }
      }

      if (i < cmd->num_commands - 1) {
        // Not the last pipe
        // Close the read end of the current pipe
        close(holder->pipes[i][READ]);

        // Redirect stdout to write to the current pipe
        int ret = dup2(holder->pipes[i][WRITE], STDOUT_FILENO);
        if (ret < 0) {
          perror("dup2");
          exit(EXIT_FAILURE);
        }
      }

      // Redirect stdin to read from a file if necessary
      if (cmd->stdin_file != NULL) {
        int flags = O_RDONLY | O_CREAT;
        int fd = open(cmd->stdin_file, flags, PERMISSION);
        if (fd < 0) {
          perror("open");
          exit(EXIT_FAILURE);
        }
        int ret = dup2(fd, STDIN_FILENO);
        if (ret < 0) {
          perror("dup2");
          exit(EXIT_FAILURE);
        }
        close(fd);
      }

      // Redirect stdout to write to a file if necessary
      if (cmd->stdout_file != NULL) {
        int flags = O_WRONLY | O_CREAT;
        if (cmd->is_file_append) {
          flags |= O_APPEND;
        } else {
          flags |= O_TRUNC;
        }
        int fd = open(cmd->stdout_file, flags, PERMISSION);
        if (fd < 0) {
          perror("open");
          exit(EXIT_FAILURE);
        }
        int ret = dup2(fd, STDOUT_FILENO);
        if (ret < 0) {
          perror("dup2");
          exit(EXIT_FAILURE);
        }
        close(fd);
      }

      // Execute the current command
      execvp(*cmd->commands[i], cmd->commands[i]);
      perror("execvp");
      exit(EXIT_FAILURE);
    }

    // put the child in its own process group
    // if (setpgid(process_id, process_id) == -1) {
    //   perror("setpgid\n");
    //   exit(EXIT_FAILURE);
    // }

    // // give terminal to the child
    // if (tcsetpgrp(STDIN_FILENO, process_id) == -1) {
    //   perror("tcsetpgrp\n");
    //   exit(EXIT_FAILURE);
    // }

    // int wstatus;
    // waitpid(process_id, &wstatus, 0);

    // // ignore SIGTTOU so that tcsetpgrp doesn't stop us
    // signal(SIGTTOU, SIG_IGN);
    // tcsetpgrp(STDIN_FILENO, getpgid(0));

    // Parent process
    if (i > 0) {
      // Not the first pipe
      // Close both ends of the previous pipe
      close(holder->pipes[i - 1][READ]);
      close(holder->pipes[i - 1][WRITE]);
    }
    if (i == holder->num_pipes) {
      // Last pipe
      // Close both ends of the current pipe
      close(holder->pipes[i][READ]);
      close(holder->pipes[i][WRITE]);
    }
  }

  // Wait for all children to terminate
  wait_for_children(cmd);
}
