CC=gcc
INC=
FLAGS=-lrt -pthread -O3

BINDIR=bin
SRCDIR=src

all: SemTest

$(BINDIR):
	mkdir -p $(BINDIR)

SemTest: $(BINDIR) $(BINDIR)/SemTest

$(BINDIR)/SemTest: $(BINDIR)/SemTest.o 
	$(CC) $(INC) $(FLAGS) $(BINDIR)/SemTest.o -o $(BINDIR)/SemTest

$(BINDIR)/SemTest.o: $(SRCDIR)/SemTest.c
	$(CC) $(INC) $(FLAGS) -c $(SRCDIR)/SemTest.c -o $(BINDIR)/SemTest.o

