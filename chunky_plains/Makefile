
obj = chunky_plains.o


all: test.exe star.exe box.exe

chunky_plains.o: chunky_plains.h chunky_plains.c
		gcc -c chunky_plains.c -o chunky_plains.o  -D__USE_INLINE__ -lauto

test.exe: test.c  chunky_plains.o 
		gcc test.c $(obj) -o test.exe -D__USE_INLINE__ -lauto
star.exe: stars.c  chunky_plains.o 
		gcc stars.c $(obj) -o star.exe -D__USE_INLINE__ -lauto
box.exe: box.c  chunky_plains.o 
		gcc box.c $(obj) -o box.exe -D__USE_INLINE__ -lauto