CC = g++
CFLAGS = -w

OBJS = main.o util.o scan.o parse.o dot.o symtab.o analyze.o code.o cgen.o

ifeq ($(OS),Windows_NT)
	RM = del
	EXE = .exe
else
	RM = rm -f
	EXE =
endif

all: tiny$(EXE) clean_tmp

tiny$(EXE): $(OBJS)
	$(CC) -o tiny$(EXE) $(OBJS) $(CFLAGS)

main.o: main.cpp globals.h util.h scan.h parse.h dot.h analyze.h cgen.h
	$(CC) -c main.cpp $(CFLAGS)

util.o: util.cpp util.h globals.h
	$(CC) -c util.cpp $(CFLAGS)

scan.o: scan.cpp scan.h util.h globals.h
	$(CC) -c scan.cpp $(CFLAGS)

parse.o: parse.cpp parse.h scan.h globals.h util.h
	$(CC) -c parse.cpp $(CFLAGS)

dot.o: dot.cpp dot.h parse.h globals.h
	$(CC) -c dot.cpp $(CFLAGS)

symtab.o: symtab.cpp symtab.h
	$(CC) -c symtab.cpp $(CFLAGS)

analyze.o: analyze.cpp globals.h symtab.h analyze.h
	$(CC) -c analyze.cpp $(CFLAGS)

code.o: code.cpp code.h globals.h
	$(CC) -c code.cpp $(CFLAGS)

cgen.o: cgen.cpp globals.h symtab.h code.h cgen.h
	$(CC) -c cgen.cpp $(CFLAGS)

clean:
	-$(RM) tiny$(EXE)
	-$(RM) $(OBJS)

clean_tmp:
	-$(RM) $(OBJS)