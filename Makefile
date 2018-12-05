CC    = cc
EXECS = newlisten newspeak datalisten dataspeak

.c.o:
	$(CC) $(CFLAGS) -w -c $*.c

all:	$(EXECS)
	@echo done.

clean:
	rm -f *.o

cleaner: clean
	rm -f $(EXECS)

