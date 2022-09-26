TARGET=bin/elevator
BUILDDIR=build
SRCDIR=src
INCLUDEDIR=inc
TESTDIR=test

CC=gcc
CFLAGS=-I$(INCLUDEDIR)
DEP=$(INCLUDEDIR)/ntk.h $(SRCDIR)/packet_reader.h $(SRCDIR)/packet_definitions.h
OBJECTS=$(BUILDDIR)/main.o $(BUILDDIR)/packet_reader.o $(BUILDDIR)/packet_definitions.o $(BUILDDIR)/ntk.o
APITEST=$(TESTDIR)/api-test.c $(SRCDIR)/packet_reader.c

#for windows
ARGS=-lcheck 

#for macos
#ARGS=-lcheck -lm -lpthread 

#for linux
#ARGS=-lcheck -lm -lrt -lpthread -lsubunit 

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)
		
$(BUILDDIR)/%.o: $(SRCDIR)/%.c $(DEP)
	$(CC) $(ARGS) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.o: $(INCLUDEDIR)/%.c $(DEP)
	$(CC) $(ARGS) $(CFLAGS) -c $< -o $@
	
test:
$(BINDIR)/api-test: $(APITEST) $(DEP)
	$(CC) $(APITEST) $(ARGS) -o $@

$(BUILDDIR)/api-test.c: $(TESTDIR)/api-test.check
	checkmk $^ > $@
	
clean:
	rm $(BUILDDIR)/*.o