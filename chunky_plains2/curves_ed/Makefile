
all: 	curves_ed_load.o curves_ed.exe

curves_ed_load.o: curves_ed_load.c
		gcc curves_ed_load.c -o curves_ed_load.o -c

curves_ed.exe: curves_ed_load.o curves_ed.c  ../chunky_plains2.h 
		gcc curves_ed_load.o curves_ed.c -o curves_ed.exe -D__USE_INLINE__ -lauto

clean:
		delete #?.(o|exe)