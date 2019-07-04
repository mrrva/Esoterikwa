
#include "runtime.h"

int main(int argc, char *argv[])
{
	size_t size = _binary_instructions_end - _binary_instructions_start;
	unsigned char *bytes = &_binary_instructions_start[0];
	struct ekwa_instruction *ekwa_list = NULL;

#ifdef RUNTIME_DEBUG
	printf("[I]: Size of instructions: %ld bytes.\n", size);
#endif
	ekwa_vars = NULL;
	ekwa_args = NULL;

	if (size < 3) {
		printf("[E]: Instructions list is empty.\n");
		return 1;
	}

	if (!ekwa_frombytecode(&ekwa_list, bytes, size)) {
		printf("[E]: Invalid instructions.\n");
		return 1;
	}

	ekwa_virtual_machine(ekwa_list);
	ekwa_instruction_clear(&ekwa_list);

	return 0;
}