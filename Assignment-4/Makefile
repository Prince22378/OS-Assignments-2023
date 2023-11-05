all:
	gcc  -m32 -no-pie -nostdlib -o fib fib.c
	gcc  -m32 -no-pie -nostdlib -o prime prime.c
	gcc  -m32 -no-pie -nostdlib -o sum sum.c
	gcc -m32 -o l simpleSmartLoader.c
	gcc -m32 -o lb simpleSmartLoaderBonus.c

clean:
	-@rm -f fib loader
