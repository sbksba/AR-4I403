CC=gcc
CFLAGS=-Wall
BIN=bin
INC=include
LIB=lib
OBJ=obj
SRC=src
LOG=log
EXEC=CANp2p

all: directories $(EXEC)

directories: ${OBJ} ${BIN} ${LIB} ${LOG}

${OBJ}:
	mkdir -p ${OBJ}

${BIN}:
	mkdir -p ${BIN}

${LIB}:
	mkdir -p ${LIB}

${LOG}:
	mkdir -p ${LOG}

# =====
# OBJET
# =====
${OBJ}/can.o: ${SRC}/can.c
	${CC} -c -o $@ $< $(CFLAGS) -I${INC} -lrt

# ============
# BIBLIOTHEQUE
# ============
${LIB}/libcanP2P.a : ${OBJ}/can.o
	ar -rs $@ $^

# ==========
# EXECUTABLE
# ==========
$(EXEC): ${LIB}/libcanP2P.a
	${CC} -o ${BIN}/$@ $^ -lrt

.PHONY: all proper clean cleanall

proper:
	rm -f *~ ${INC}/*~ ${SRC}/*~ ${LOG}/*.log

clean: proper
	rm -f ${OBJ}/* ${BIN}/* ${LIB}/*

cleanall: clean
	rm -r ${OBJ} ${BIN} ${LIB}
