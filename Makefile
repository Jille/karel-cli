#CFLAGS=	-Wall -Wmissing-prototypes -Werror -g
#CC=	cc
CFLAGS=	-Wall -Werror -g
CC=	g++

karel: karel.o kernel.o gui.o
	$(CC) -o karel karel.o kernel.o gui.o

nckarel: karel.o kernel.o ncgui.o
	$(CC) -o nckarel -lncurses karel.o kernel.o ncgui.o

all: clean karel nckarel

karel.o: karel.c
	$(CC) -c $(CFLAGS) karel.c

kernel.o: kernel.c kernel.h
	$(CC) -c $(CFLAGS) kernel.c

gui.o: gui.c kernel.h
	$(CC) -c $(CFLAGS) gui.c

ncgui.o: ncgui.c kernel.h
	$(CC) -c $(CFLAGS) ncgui.c

clean:
	rm -f karel nckarel karel.o kernel.o gui.o ncgui.o
