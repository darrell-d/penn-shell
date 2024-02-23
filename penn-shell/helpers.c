#include "helpers.h"

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
    pid_t child_pid = waitpid(-1, &status, 0);
    if (child_pid == -1) {
      perror("waitpid");
      exit(EXIT_FAILURE);
    }
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

// // Execute command (sequential)
// // TODO: refactor (split into two functions?) so it doesn't execute
// everything sequentially void execute_command(struct parsed_command *cmd,
// struct pipe_holder *holder, int pipe_index) {
//   // Make a pipe
//   int pipe_fds[2];
//   if (pipe(pipe_fds) < 0) {
//     perror("pipe error");
//     exit(EXIT_FAILURE);
//   }
//   holder->pipes[pipe_index][0] = pipe_fds[0];
//   holder->pipes[pipe_index][1] = pipe_fds[1];

//   // Fork a child
//   pid_t process_id = fork();
//   if (process_id < 0) {
//     perror("fork");
//     exit(EXIT_FAILURE);
//   } else if (process_id == 0) {
//     // Child process
//     if (pipe_index > 0) {
//       // Not the first pipe
//       // Close the write end of the previous pipe
//       close(holder->pipes[pipe_index - 1][WRITE]);

//       // Redirect stdin to read from the previous pipe
//       int ret = dup2(holder->pipes[pipe_index - 1][READ], STDIN_FILENO);
//       if (ret < 0) {
//         perror("dup2");
//         exit(EXIT_FAILURE);
//       }
//     }

//     if (pipe_index < holder->num_pipes) {
//       // Not the last pipe
//       // Close the read end of the current pipe
//       close(holder->pipes[pipe_index][READ]);

//       // Redirect stdout to write to the current pipe
//       int ret = dup2(holder->pipes[pipe_index][WRITE], STDOUT_FILENO);
//       if (ret < 0) {
//         perror("dup2");
//         exit(EXIT_FAILURE);
//       }
//     }

//     // Redirect stdin to read from a file if necessary
//     if (cmd->stdin_file != NULL) {
//       int flags = O_RDONLY | O_CREAT;
//       int fd = open(cmd->stdin_file, flags, PERMISSION);
//       if (fd < 0) {
//         perror("open");
//         exit(EXIT_FAILURE);
//       }
//       int ret = dup2(fd, STDIN_FILENO);
//       if (ret < 0) {
//         perror("dup2");
//         exit(EXIT_FAILURE);
//       }
//       close(fd);
//     }

//     // Redirect stdout to write to a file if necessary
//     if (cmd->stdout_file != NULL) {
//       int flags = O_WRONLY | O_CREAT;
//       if (cmd->is_file_append) {
//         flags |= O_APPEND;
//       } else {
//         flags |= O_TRUNC;
//       }
//       int fd = open(cmd->stdout_file, flags, PERMISSION);
//       if (fd < 0) {
//         perror("open");
//         exit(EXIT_FAILURE);
//       }
//       int ret = dup2(fd, STDOUT_FILENO);
//       if (ret < 0) {
//         perror("dup2");
//         exit(EXIT_FAILURE);
//       }
//       close(fd);
//     }

//     // Execute the current command
//     execvp(*cmd->commands[pipe_index], cmd->commands[pipe_index]);
//     perror("execvp");
//     exit(EXIT_FAILURE);
//   }

//   // Parent process
//   if (pipe_index > 0) {
//     // Not the first pipe
//     // Close both ends of the previous pipe
//     close(holder->pipes[pipe_index - 1][READ]);
//     close(holder->pipes[pipe_index - 1][WRITE]);
//   }
//   if (pipe_index == holder->num_pipes) {
//     // Last pipe
//     // Close both ends of the current pipe
//     close(holder->pipes[pipe_index][READ]);
//     close(holder->pipes[pipe_index][WRITE]);
//   }

//   // Wait for the child process to terminate
//   int status;
//   pid_t child_pid = waitpid(process_id, &status, 0);
//   if (child_pid == -1) {
//     perror("waitpid");
//     exit(EXIT_FAILURE);
//   }
// }
