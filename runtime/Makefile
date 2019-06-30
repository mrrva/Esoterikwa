
all:
	gcc -c $@ ekwa_main.c ekwa_instructions.c -o ekwa_runtime -march=x86-64
	ld -r -b binary -o instructions.o instructions
	gcc ekwa_runtime instructions.o -o program -march=x86-64

clear:
	rm -f instructions.o ekwa_runtime program