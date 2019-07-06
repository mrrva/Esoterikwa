
soft:
	gcc -c ekwa_main.c -o ekwa_runtime -march=x86-64
	ld -r -b binary -o instructions.o instructions
	gcc ekwa_runtime instructions.o ekwa_instructions.c ekwa_tokens.c \
		ekwa_vm.c -ldl -o program -march=x86-64

tmodule:
	gcc -c -Wall -fPIC tmodule.c
	gcc -shared -o ekwatmodule.so tmodule.o
	rm -f tmodule.o

clear:
	rm -f instructions.o ekwa_runtime program tmodule.o \
		ekwatmodule.so