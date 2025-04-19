#include <stdio.h>
#include <stdlib.h>

int main() {
  double sum = 0.0;
  int count =  0;

  double curr;
  while(scanf("%lf", &curr) == 1) {
    sum += curr;
    count++;
  }

  double average = 0.0;
  if (count != 0) {
    // no divide by zero error
    average = sum / count;
  }
  printf("Average value is: %lf\n", average);

  return EXIT_SUCCESS;
}
