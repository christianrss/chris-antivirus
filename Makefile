CFLAGS=-O2 -Wall -std=c23 -m32
LDFLAGS=-lbu

.PHONY: all clean

all: clean bin/chris_antivirus

bin:
	mkdir -p bin

bin/chris_antivirus: bin/chris_antivirus.o | bin
	cc $(CFLAGS) $^ -o $@ $(LDFLAGS)

bin/chris_antivirus.o: chris_antivirus.c chris_antivirus.h | bin
	cc $(CFLAGS) -c $< -o $@

clean:
	rm -f ./bin/*.o ./bin/chris_antivirus