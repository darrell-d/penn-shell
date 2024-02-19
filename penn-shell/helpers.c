#include "helpers.h"

// Dynamically allocate a pipe_holder struct
struct pipe_holder *create_pipe_holder(size_t num_pipes) {
  struct pipe_holder *holder = malloc(sizeof(struct pipe_holder) + num_pipes * sizeof(size_t[2]));
  if (holder == NULL) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }
  holder->num_pipes = num_pipes;
  return holder;
}

// Base case for pipes
// Open a pipe and fork a child process
// Within the child process:
//   Close the read end of the pipe
//   Redirect stdout to the write end of the pipe
//  Execute the first command
// Within the parent process:
//  Close the write end of the pipe
//  Wait for the child process to terminate
void execute_command(struct parsed_command *cmd, struct pipe_holder *holder, int pipe_index) {
  // Make a pipe
  int pipe_fds[2];
  if (pipe(pipe_fds) < 0) {
    perror("pipe error");
    exit(EXIT_FAILURE);
  }
  holder->pipes[pipe_index][0] = pipe_fds[0];
  holder->pipes[pipe_index][1] = pipe_fds[1];

  // Fork a child
  pid_t process_id = fork();
  if (process_id < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  } else if (process_id == 0) {
    // Child process
    if (pipe_index > 0) {
      // Not the first pipe
      // Close the write end of the previous pipe
      close(holder->pipes[pipe_index - 1][WRITE]);

      // Redirect stdin to read from the previous pipe
      int ret = dup2(holder->pipes[pipe_index - 1][READ], STDIN_FILENO);
      if (ret < 0) {
        perror("dup2");
        exit(EXIT_FAILURE);
      }
    }

    if (pipe_index < holder->num_pipes) {
      // Not the last pipe
      // Close the read end of the current pipe
      close(holder->pipes[pipe_index][READ]);

      // Redirect stdout to write to the current pipe
      int ret = dup2(holder->pipes[pipe_index][WRITE], STDOUT_FILENO);
      if (ret < 0) {
        perror("dup2");
        exit(EXIT_FAILURE);
      }
    }

    // Execute the current command
    execvp(*cmd->commands[pipe_index], cmd->commands[pipe_index]);
    perror("execvp");
    exit(EXIT_FAILURE);
  }

  // Parent process
  if (pipe_index > 0) {
    // Not the first pipe
    // Close both ends of the previous pipe
    close(holder->pipes[pipe_index - 1][READ]);
    close(holder->pipes[pipe_index - 1][WRITE]);
  }
  if (pipe_index == holder->num_pipes) {
    // Last pipe
    // Close both ends of the current pipe
    close(holder->pipes[pipe_index][READ]);
    close(holder->pipes[pipe_index][WRITE]);
  }

  // Wait for the child process to terminate
  int status;
  pid_t child_pid = waitpid(process_id, &status, 0);
  if (child_pid == -1) {
    perror("waitpid");
    exit(EXIT_FAILURE);
  }
}
