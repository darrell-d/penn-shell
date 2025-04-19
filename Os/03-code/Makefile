######################################################################
# Basic Makefile example with minimal features shown

# marks "clean" and "all" as fake targets that generate a file
# called "clean" or "all" respectively
.PHONY : clean all
PROGS = status_check no_hang hang delay_sigint numbers interrupt  suspend_sigint

all: $(PROGS)

status_check: status_check.c
	gcc-12 -Wall -Werror --std=c2x -g -o status_check status_check.c

hang: hang.c
	gcc-12 -Wall -Werror --std=c2x -g -o hang hang.c

delay_sigint: delay_sigint.c
	gcc-12 -Wall -Werror --std=c2x -g -o delay_sigint delay_sigint.c

numbers: numbers.c
	gcc-12 -Wall -Werror --std=c2x -g -o numbers numbers.c

suspend_sigint: suspend_sigint.c
	gcc-12 -Wall -Werror --std=c2x -g -o suspend_sigint suspend_sigint.c

interrupt: interrupt.c
	gcc-12 -Wall -Werror --std=c2x -g -o interrupt interrupt.c

no_hang: no_hang.c
	gcc-12 -Wall -Werror --std=c2x -g -o no_hang no_hang.c

clean:
	rm $(PROGS)
