#ifndef JOB_H_
#define JOB_H_

#include <stdint.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "./parser.h"

// define new type "job id"
typedef uint64_t jid_t;

// Represents a job
typedef struct job_st {
  uint64_t id;
  struct parsed_command* cmd;
  pid_t* pids; // array of pids of size cmd->num_commands
  // TODO: you probably want to add some more fields here
} job;

#endif  // JOB_H_
