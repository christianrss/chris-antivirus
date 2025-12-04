flags=-O3 -Wall -std=c23 -m32 -I.
ldflags=-fPIC -shared -ldl -D_GNU_SOURCE
libdir=/lib/x86_64-linux-gnu/
incdir=/usr/include

all: clean bin bin/chrisutils.so

bin:
	mkdir -p bin

install: bin/chrisutils.so | bin
	cp bin/chrisutils.so $(libdir)/libbu.so
	cp chrisutils.h $(incdir)
	ldconfig

bin/chrisutils.so: bin/chrisutils.o | bin
	cc ${flags} $^ -o $@ ${ldflags}

bin/chrisutils.o: chrisutils.c | bin
	cc ${flags} -c $^ -o $@ $(ldflags)

clean:
	rm -f ./bin/*.o ./bin/*.so