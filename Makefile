#CFLAGS=	-Wall -Wmissing-prototypes -Werror -g
#CC=	cc
CFLAGS=	-Wall -Werror -g
CC=	g++

CFILES := gui.c karel.c kernel.c ncgui.c
OFILES := $(CFILES:%.c=%.o)

karel: karel.o kernel.o gui.o
	$(CC) -o karel $^

nckarel: karel.o kernel.o ncgui.o
	$(CC) -o nckarel -lncurses $^

ifeq ($(MAKECMDGOALS),)
-include Makefile.dep
endif
ifneq ($(filter-out clean, $(MAKECMDGOALS)),)
-include Makefile.dep
endif

.PHONY: clean all depend
.SUFFIXES:

all: clean karel nckarel

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

Makefile.dep: $(CFILES)
	for i in $(^); do $(CC) $(CFLAGS) -MM "$${i}" -MT `basename $${i%.*}`.o; done > $@

clean:
	rm -f karel nckarel $(OFILES) Makefile.dep
