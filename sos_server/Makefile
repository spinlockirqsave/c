
PROGS =	sossrv
LIBS  = -pthread
all:	${PROGS}
CFLAGS = -g
sossrv:	sossrv.o
	${CC} ${CFLAGS} $@.c -o $@ ${LIBS}

soscli:	soscli.o
	${CC} ${CFLAGS} $@.c -o $@ ${LIBS}

clean:
		rm -f ${PROGS} ${CLEANFILES}
