
obj = chunky_plains2.o

all: test.exe star.exe box.exe circle.exe donenut.exe triangle.exe donenut2.exe tree.exe mandel.exe shadow.exe reflect.exe

chunky_plains2.o: chunky_plains2.h chunky_plains2.c
		gcc -c chunky_plains2.c -o chunky_plains2.o -D__USE_INLINE__ -lauto

test.exe: test.c  chunky_plains2.o
		gcc test.c $(obj) -o test.exe  -D__USE_INLINE__ -lauto

star.exe: stars.c  chunky_plains2.o 
		gcc stars.c $(obj) -o star.exe -D__USE_INLINE__ -lauto

box.exe: box.c  chunky_plains2.o 
		gcc box.c $(obj) -o box.exe -D__USE_INLINE__ -lauto

donenut.exe: donenut.c  chunky_plains2.o 
		gcc donenut.c $(obj) -o donenut.exe -D__USE_INLINE__ -lauto

circle.exe: circle.c  chunky_plains2.o 
		gcc circle.c $(obj) -o circle.exe -D__USE_INLINE__ -lauto

triangle.exe: triangle.c  chunky_plains2.o 
		gcc triangle.c $(obj) -o triangle.exe -D__USE_INLINE__ -lauto

donenut2.exe: donenut2.c  chunky_plains2.o 
		gcc donenut2.c $(obj) -o donenut2.exe -D__USE_INLINE__ -lauto

tree.exe: tree.c  chunky_plains2.o 
		gcc tree.c $(obj) -o tree.exe -D__USE_INLINE__ -lauto

mandel.exe: mandel.c  chunky_plains2.o 
		gcc mandel.c $(obj) -o mandel.exe -D__USE_INLINE__ -lauto

shadow.exe: shadow.c  chunky_plains2.o 
		gcc shadow.c $(obj) -o shadow.exe -D__USE_INLINE__ -lauto

reflect.exe: reflect.c  chunky_plains2.o 
		gcc reflect.c $(obj) -o reflect.exe -D__USE_INLINE__ -lauto

curves2.exe: curves2.c  chunky_plains2.o 
		gcc curves2.c $(obj) -o curves2.exe -D__USE_INLINE__ -lauto

curves3.exe: curves3.c  chunky_plains2.o 
		gcc curves3.c $(obj) -o curves3.exe -D__USE_INLINE__ -lauto

curves4.exe: curves4.c  chunky_plains2.o 
		gcc curves4.c $(obj) -o curves4.exe -D__USE_INLINE__ -lauto

curves5.exe: curves5.c  chunky_plains2.o 
		gcc curves5.c $(obj) -o curves5.exe -D__USE_INLINE__ -lauto

curves_ed.exe: curves_ed.c  chunky_plains2.o 
		gcc curves_ed.c $(obj) -o curves_ed.exe -D__USE_INLINE__ -lauto

clean:
		delete #?(.o|.exe)