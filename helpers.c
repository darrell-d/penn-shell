#include "helpers.h"

int count_items(char* array[]) {
  int counter = 0;

  while (array[counter] != NULL) {
    counter++;
  }
  return counter;
}

// char* get_cmd_args(char** cmd, char* cmdArgs) {

// }