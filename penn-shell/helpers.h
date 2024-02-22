#ifndef HELPERS_H
#define HELPERS_H
#define READ 0
#define WRITE 1
#define PERMISSION 0644

#include <fcntl.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "parser.h"
#ifndef PROMPT
#define PROMPT "penn-shredder# "
#endif

// Struct to hold pipes
struct pipe_holder {
  size_t num_pipes;
  int pipes[][2];
};

// Dynamically allocate a pipe_holder struct
struct pipe_holder* create_pipe_holder(size_t num_pipes);

// Execute a command
void execute_command(struct parsed_command* cmd,
                     struct pipe_holder* holder,
                     int pipe_index);

// Signal handlers
void setup_sig_handlers();
void signal_silencer(int signo);

#endif /* HELPERS_H */