#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// This is pseudo code that does not compile
// this is just to demonstrate critical sections
// and how signals can interfere and be dangerous

typedef struct node_st {
  struct node_st* next;
  float value;
} node;

typedef struct list_st {
  node* head;
  node* tail; 
  size_t len;
} list;

void list_push(list* this, float to_push) {
  node* n = malloc(sizeof(Node));
  if (node == NULL) {
    exit(EXIT_FAILURE);
  }
  *n = (node) {
    .next = NULL,
    .value = to_push,
  };
  this->tail->next = node;
  this->tail = node;
  this->len += 1;
}

list global_list = (list) {
  .head = NULL,
  .tail = NULL,
  .len = 0U,
}

void sig_handler(int signo) {
  if (signo == SIGINT) {
    list_push(&global_list, 4.48f);
  }
}

int main() {
  signal(SIGINT, handler);
  float f = 0.0f;
  while(list_size(&global_list) < 20) {
    read_float(stdin, &f);
    list_push(&global_list, f);
  }

  // other stuff
}

