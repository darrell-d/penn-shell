#ifndef HELPERS_H
#define HELPERS_H
#define READ 0
#define WRITE 1
#define PERMISSION 0644

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "parser.h"

// Struct to hold pipes
struct pipe_holder {
  size_t num_pipes;
  int pipes[][2];
};

// Dynamically allocate a pipe_holder struct
struct pipe_holder *create_pipe_holder(size_t num_pipes);

// // Wait for all children to terminate
// void wait_for_children(struct parsed_command *cmd);

// Execute all commands in parallel
void execute_commands(struct parsed_command *cmd, struct pipe_holder *holder);

// // Execute a command
// void execute_command(struct parsed_command *cmd, struct pipe_holder *holder, int pipe_index);

#endif /* HELPERS_H */