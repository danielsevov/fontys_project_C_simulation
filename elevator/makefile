CC=cl.exe
CFLAGS=-I.
SRCDIR=src/
BUILDDIR=build/
INCLUDEDIR=inc/
TESTDIR=test/
TARGETDIR=bin/

all: $(TARGETDIR)elevator

$(TARGETDIR)elevator: $(BUILDDIR)OSCO.obj $(BUILDDIR)ntk_5.4.obj $(BUILDDIR)elevator.obj $(BUILDDIR)packet_definitions.obj $(BUILDDIR)packet_reader.obj $(BUILDDIR)socket_client.obj
	$(CC) $(BUILDDIR)elevator.obj $(BUILDDIR)ntk_5.4.obj $(BUILDDIR)OSCO.obj $(BUILDDIR)packet_definitions.obj $(BUILDDIR)packet_reader.obj $(BUILDDIR)socket_client.obj /Fec:bin\elevator.exe

$(BUILDDIR)elevator.obj: $(SRCDIR)elevator.c  $(INCLUDEDIR)elevator.h $(INCLUDEDIR)ntk_5.4.h $(INCLUDEDIR)OSCO.h $(BUILDDIR)packet_definitions.obj $(BUILDDIR)packet_reader.obj $(BUILDDIR)socket_client.obj
	$(CC) -Iinc -c $(SRCDIR)elevator.c /Foc:build\elevator.obj

$(BUILDDIR)ntk_5.4.obj: $(SRCDIR)ntk_5.4.c  $(INCLUDEDIR)ntk_5.4.h
	$(CC) -Iinc -c $(SRCDIR)ntk_5.4.c /Foc:build\ntk_5.4.obj

$(BUILDDIR)OSCO.obj: $(SRCDIR)OSCO.c  $(INCLUDEDIR)OSCO.h
	$(CC) -Iinc -c $(SRCDIR)OSCO.c /Foc:build\OSCO.obj

$(BUILDDIR)packet_definitions.obj: $(SRCDIR)packet_definitions.c $(INCLUDEDIR)packet_definitions.h
	$(CC) Iinc -c $(SRCDIR)packet_definitions.c /Foc:build\packet_definitions.obj

$(BUILDDIR)packet_reader.obj: $(SRCDIR)packet_reader.c $(INCLUDEDIR)packet_reader.h $(BUILDDIR)packet_definitions.obj
	$(CC) Iinc -c $(SRCDIR)packet_definitions.c $(BUILDDIR)packet_definitions.obj /Foc:build\packet_reader.obj

$(BUILDDIR)socket_client.obj: $(SRCDIR)socket_client.c $(INCLUDEDIR)socket_client.h
	$(CC) Iinc -c $(SRCDIR)socket_client.c /Foc:build\socket_client.obj