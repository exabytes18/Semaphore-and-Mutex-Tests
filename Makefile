CC=gcc
INC=
FLAGS=-lrt -pthread -O3

BINDIR=bin
SRCDIR=src

all: SemTest MutexTest ContextSwitchTest

$(BINDIR):
	mkdir -p $(BINDIR)

SemTest: $(BINDIR) $(BINDIR)/SemTest

$(BINDIR)/SemTest: $(BINDIR)/SemTest.o 
	$(CC) $(INC) $(FLAGS) $(BINDIR)/SemTest.o -o $(BINDIR)/SemTest

$(BINDIR)/SemTest.o: $(SRCDIR)/SemTest.c
	$(CC) $(INC) $(FLAGS) -c $(SRCDIR)/SemTest.c -o $(BINDIR)/SemTest.o

MutexTest: $(BINDIR) $(BINDIR)/MutexTest

$(BINDIR)/MutexTest: $(BINDIR)/MutexTest.o 
	$(CC) $(INC) $(FLAGS) $(BINDIR)/MutexTest.o -o $(BINDIR)/MutexTest

$(BINDIR)/MutexTest.o: $(SRCDIR)/MutexTest.c
	$(CC) $(INC) $(FLAGS) -c $(SRCDIR)/MutexTest.c -o $(BINDIR)/MutexTest.o

ContextSwitchTest: $(BINDIR) $(BINDIR)/ContextSwitchTest

$(BINDIR)/ContextSwitchTest: $(BINDIR)/ContextSwitchTest.o 
	$(CC) $(INC) $(FLAGS) $(BINDIR)/ContextSwitchTest.o -o $(BINDIR)/ContextSwitchTest

$(BINDIR)/ContextSwitchTest.o: $(SRCDIR)/ContextSwitchTest.c
	$(CC) $(INC) $(FLAGS) -c $(SRCDIR)/ContextSwitchTest.c -o $(BINDIR)/ContextSwitchTest.o

