######################################################################
# Basic Makefile example with minimal features shown

# marks "clean" and "all" as fake targets that generate a file
# called "clean" or "all" respectively
.PHONY : clean all

all: ctrlc impatient no_sleep demo exec_example broken_autograder

demo: demo.c
	gcc-12 -Wall -Werror --std=c2x -g -o demo demo.c

ctrlc: ctrlc.c
	gcc-12 -Wall -Werror --std=c2x -g -o ctrlc ctrlc.c

impatient: impatient.c
	gcc-12 -Wall -Werror --std=c2x -g -o impatient impatient.c	

no_sleep: no_sleep.c
	gcc-12 -Wall -Werror --std=c2x -g -o no_sleep no_sleep.c

broken_autograder: broken_autograder.c
	gcc-12 -Wall -Werror --std=c2x -g -o broken_autograder broken_autograder.c

exec_example: exec_example.c
	gcc-12 -Wall -Werror --std=c2x -g -o exec_example exec_example.c

clean:
	rm ctrlc impatient no_sleep demo exec_example broken_autograder
