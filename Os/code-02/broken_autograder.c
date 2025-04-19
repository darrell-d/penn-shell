#include <unistd.h>    // for fork and execve
#include <sys/types.h> // for pid_t
#include <stdlib.h> // for exit(), EXIT_SUCCESS and EXIT_FAILURE
#include <stdio.h>  // for perror()

int main(int argc, char** argv) {
  char* envp[] = { NULL };
  // fork a process to exec clang

  pid_t clang_pid = fork();

  // check for errors
  if (clang_pid < 0) {
    perror("Could not fork process to run clang");
    exit(EXIT_FAILURE);
  }

  if (clang_pid == 0) {
    // we are the child
    char* clang_argv[] = {"/bin/clang", "-Wall", "-g", "-o", "hello", "hello_world.c", NULL};
    execve(clang_argv[0], clang_argv, envp);

    // if we got here, then execve failed
    perror("could not exec clang");
    exit(EXIT_FAILURE);
  }

  // implicity clang_pid != 0
  // parent process executes this code

  // create a process to run the compiled program
  pid_t hello_pid = fork();

  if (hello_pid < 0) {
    perror("Could not fork process to run ./hello");
    exit(EXIT_FAILURE);
  }

  if (hello_pid == 0) {
    // the process created by fork
    char* hello_argv[] = {"./hello", NULL};
    execve(hello_argv[0], hello_argv, envp);

    // error if we got here, exec should not return
    perror("could not exec ./hello");
    exit(EXIT_FAILURE);
  }

  // Parent is done
  
  return EXIT_SUCCESS;
}
