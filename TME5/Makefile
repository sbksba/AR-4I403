GCC=gcc
CC=mpicc
CFLAGS=
RUN=mpirun

all: RA

# ==========
# EXECUTABLE
# ==========
RA: ra.c
	${CC} -o $@ $^ 2> errorRA.log

.PHONY: all proper clean

proper:
	rm -f *~ *.log

clean: proper
	rm -f RA

bench:
	@echo "## RA ##"
	${RUN} -n 7 ./RA
	@echo ""
