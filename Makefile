######################################################################
#
#                       Author: Hannah Pan
#                       Date:   01/13/2021
#
# The autograder will run the following command to build the program:
#
#       make -B
#
# To build a version that does not call kill(2), it will run:
#
#       make -B CPPFLAGS=-DEC_NOKILL
#
######################################################################

# name of the program to build
#
PROG=penn-shell

PROMPT='"$(PROG)> "'

# Remove -DNDEBUG during development if assert(3) is used
#
override CPPFLAGS += -DNDEBUG -DPROMPT=$(PROMPT)

CC = gcc-12

# Replace -O1 with -g for a debug version during development
#
CFLAGS += -g -Wall -Werror -Wpedantic -I. -I.. --std=c2x
CXXFLAGS += -g -Wall -Werror -Wpedantic -I. -I.. --std=c++23

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
HEADERS = $(wildcard *.h)

.PHONY : all clean tidy-check format

all: $(PROG) tidy-check

$(PROG) : $(OBJS) $(HEADERS)
	$(CC) -o $@ $(OBJS) parser.o

%.o: %.c $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

clean :
	$(RM) $(OBJS) $(PROG)

# Explanantion of args:
#   -extra-arg=-std=c2x : specifies to check with C23
#   -warnings-as-error= : marks all warnings as errors
#   -checks= : specifies which checks to look for
#     cert-* turns on all errors related to CERT security checks.
#     readability-* turns on all errors related to making code more readable
#     bugprone-* turns on errors that will help identify frequent beginner bugs
tidy-check: 
	clang-tidy \
        --extra-arg=--std=c2x \
	--extra-arg=-D_POSIX_C_SOURCE \
        -warnings-as-errors=* \
        -checks=,cert-*,readability-*,bugprone-*,-cert-err33-c,-bugprone-easily-swappable-parameters,-bugprone-signal-handler,-cert-sig30-c,-bugprone-reserved-identifier,-cert-dcl37-c,-cert-dcl51-cpp\
        -header-filter=.* \
        $(SRCS) $(HEADERS)

format:
	clang-format -i --verbose --style=Chromium $(SRCS) $(HEADERS)

