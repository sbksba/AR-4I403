GCC=gcc
CC=mpicc
CFLAGS=
RUN=mpirun

all: PHASE

# ==========
# EXECUTABLE
# ==========
PHASE: phase.c
	${CC} -o $@ $^ 2> errorPHASE.log

.PHONY: all proper clean

proper:
	rm -f *~ *.log

clean: proper
	rm -f PHASE

bench:
	@echo "## PHASE ##"
	${RUN} -n 7 ./PHASE
	@echo ""
