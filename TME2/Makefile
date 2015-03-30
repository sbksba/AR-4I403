GCC=gcc
CC=mpicc
CFLAGS=
RUN=mpirun

all: TESTPHILO

# ==========
# EXECUTABLE
# ==========
PHILO: philosophe.c
	${GCC} -o $@ $^ 2> errorPHILO.log

TESTPHILO: testPhilo.c
	${CC} -o $@ $^ 2> errorTESTPHILO.log

.PHONY: all proper clean

proper:
	rm -f *~ *.log

clean: proper
	rm -f PHILO TESTPHILO

bench:
	@echo "## TESTPHILO ##"
	${RUN} -n 5 ./TESTPHILO
	@echo ""
