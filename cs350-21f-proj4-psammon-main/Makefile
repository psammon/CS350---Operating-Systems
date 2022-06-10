sleep_seconds=3
ecode=0

test1=test1-4
test2=test5-7
test3=test8-10
test4=test11-14

all: my_mem_lib.a $(test1) $(test2) $(test3) $(test4)
	
my_mem_lib.a:  my_mem_allocator.o my_malloc.o my_free.o my_realloc.o
	ar rcs $@ $^

my_mem_allocator.o: my_mem_allocator.c my_mem_allocator.h 
	gcc -c my_mem_allocator.c

my_malloc.o: my_malloc.c my_mem_allocator.h my_mem_allocator.c
	gcc -c my_malloc.c

my_free.o: my_free.c my_mem_allocator.h 
	gcc -c my_free.c

my_realloc.o: my_realloc.c my_mem_allocator.h 
	gcc -c my_realloc.c


$(test1): $(test1).o my_mem_lib.a
	gcc $(test1).o my_mem_lib.a -o $(test1)

$(test2): $(test2).o my_mem_lib.a
	gcc $(test2).o my_mem_lib.a -o $(test2)

$(test3): $(test3).o my_mem_lib.a
	gcc $(test3).o my_mem_lib.a -o $(test3)

$(test4): $(test4).o my_mem_lib.a
	gcc $(test4).o my_mem_lib.a -o $(test4)

$(test1).o: $(test1).c my_mem_allocator.h test.h
	gcc -c $(test1).c

$(test2).o: $(test2).c my_mem_allocator.h test.h
	gcc -c $(test2).c

$(test3).o: $(test3).c my_mem_allocator.h test.h
	gcc -c $(test3).c

$(test4).o: $(test4).c my_mem_allocator.h test.h
	gcc -c $(test4).c

clean:
	rm -f  *.a *.o $(test1) $(test2) $(test3) $(test4) 
