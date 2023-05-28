PREFIX = /usr/local
CFLAGS = --std=c99
RM = rm -fP

all: opts libmguard

opts:
	@echo "build options:"
	@echo "CFLAGS = ${CFLAGS}"
	@echo "CC     = ${CC}"
	@echo "PREFIX = ${PREFIX}"

libmguard:
	$(CC) $(CFLAGS) -c libmguard.c -o libmguard.o
	ar -cvq libmguard.a libmguard.o

test: all
	$(CC) $(CFLAGS) test.c libmguard.a -o test
	./test

install: all
	mkdir -p $(PREFIX)/include
	mkdir -p $(PREFIX)/lib

	cp libmguard.h $(PREFIX)/include/libmguard.h
	cp libmguard.a $(PREFIX)/lib/libmguard.a

uninstall:
	$(RM) $(PREFIX)/include/libmguard.h
	$(RM) $(PREFIX)/lib/libmguard.a

clean:
	$(RM) *.o
	$(RM) *.a
	$(RM) test
