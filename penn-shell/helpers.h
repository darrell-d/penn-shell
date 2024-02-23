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

// Signal handlers
void setup_sig_handlers();
void signal_silencer(int signo);
void prompt_user();

// Execute all commands in parallel
void execute_commands(struct parsed_command* cmd, struct pipe_holder* holder);
void child_bg_sig_handler(int signo);

#endif /* HELPERS_H */