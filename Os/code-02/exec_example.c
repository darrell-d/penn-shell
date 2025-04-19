#include <stdio.h>   // for printf() and perror()
#include <unistd.h>  // for execve()
#include <stdlib.h>  // for exit() and EXIT_FAILURE

int main(int argc, char* argv[]) {
  char* args[] = {"/bin/cat", "exec_example.c", NULL};
  char* envp[] = { NULL };


  // even if allocate a bunch of stuff
  // and don't free it, exec will clean it up for us
  // run this program under valgrind to see!
  // valgrind --trace-children=yes --leak-check=full ./exec_example
  //                ^------- needed to trace after the exec
  char* gone = malloc(1000 * sizeof(char));

  execve(args[0], args, envp);
  
  // could check for error manually
  free(gone);
  perror("execve failed\n");
  exit(EXIT_FAILURE);

  printf("????????????????????????????????????????\n");
  printf("THIS CODE NEVER RUNS, EXECVE ONLY RETURNS IF THERE IS AN ERROR\n");
  return EXIT_FAILURE;
}
