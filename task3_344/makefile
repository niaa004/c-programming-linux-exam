INCLDIR = ./include
CC = gcc
CFLAGS = -O2
CFLAGS += -I$(INCLDIR)

OBJDIR = obj


_DEPS = pgdbglogger.h flightdata.h passenger.h flight.h menu.h
DEPS = $(patsubst %,$(INCLDIR)/%,$(_DEPS))


_OBJS = pgdbglogger.o passenger.o flight.o task3_main.o
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


task3: $(OBJS) 
	gcc -o $@ $^ $(CFLAGS)


.PHONY: clean
clean: 
	rm -f $(OBJDIR)/*.o *~ core $(INCLDIR)/*~
