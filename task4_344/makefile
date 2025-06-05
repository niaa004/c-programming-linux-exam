INCLDIR = ./include
CC = gcc
## Added -include stdio.h to compile dbj2.c
CFLAGS = -O2 -include stdio.h
## Added -pthread (POSIX) 
CFLAGS += -pthread -I$(INCLDIR)

OBJDIR = obj

## No header file for this task
_DEPS = 
DEPS = $(patsubst %,$(INCLDIR)/%,$(_DEPS))

_OBJS = task4_threads.o dbj2.o tea.o
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

task4: $(OBJS) 
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean
clean: 
	rm -f $(OBJDIR)/*.o *~ core $(INCLDIR)/*~

