
all:
	gcc -c ekwa_main.c -o ekwa_runtime -march=x86-64
	ld -r -b binary -o instructions.o instructions
	gcc ekwa_runtime instructions.o ekwa_instructions.c ekwa_tokens.c \
		ekwa_vm.c -o program -march=x86-64

clear:
	rm -f instructions.o ekwa_runtime program