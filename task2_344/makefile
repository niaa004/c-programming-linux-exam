INCLDIR = ./include
CC = gcc
## included stdio.h 
CFLAGS = -O2 -include stdio.h
CFLAGS += -I$(INCLDIR)

OBJDIR = obj

_DEPS = task2_metadata.h task2_hash.h task2_count.h task2_sum.h pgdbglogger.h
DEPS = $(patsubst %,$(INCLDIR)/%,$(_DEPS))

_OBJS = task2_main.o task2_hash.o task2_count.o task2_sum.o pgdbglogger.o
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

task2: $(OBJS) 
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean
clean: 
	rm -f $(OBJDIR)/*.o *~ core $(INCLDIR)/*~
