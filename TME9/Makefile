GCC=gcc
CC=mpicc
CFLAGS=
RUN=mpirun

all: ROBERTS ARBRE

# ==========
# EXECUTABLE
# ==========
ROBERTS: roberts.c
	${CC} -o $@ $^ #2> errorROBERTS.log

ARBRE: arbre.c
	${CC} -o $@ $^ #2> errorARBRE.log

.PHONY: all proper clean

proper:
	rm -f *~ *.log

clean: proper
	rm -f ROBERTS ARBRE
