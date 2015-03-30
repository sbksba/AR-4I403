GCC=gcc
CC=mpicc
CFLAGS=
RUN=mpirun

all: ABRMIN ECHO

# ==========
# EXECUTABLE
# ==========
ABRMIN: abrMin.c
	${CC} -o $@ $^ 2> errorABRMIN.log

ECHO: echo.c
	${CC} -o $@ $^ 2> errorECHO.log

.PHONY: all proper clean

proper:
	rm -f *~ *.log

clean: proper
	rm -f ABRMIN ECHO

bench:
	@echo "## ABRMIN ##"
	${RUN} -n 7 ./ABRMIN
	@echo ""
	@echo "## ECHO ##"
	${RUN} -n 7 ./ECHO
	@echo ""
