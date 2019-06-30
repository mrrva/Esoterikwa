
#include "include/runtime.h"

void
ekwa_virtual_machine(struct ekwa_instruction *list)
{
	struct ekwa_instruction *ptr = list;
	unsigned char buffer[MAXBUFFER_LEN];

	if (!list || list == NULL) {
		printf("[E]: Instrunctions struct is empty.\n");
		exit(1);
	}

	while (ptr && ptr != NULL) {
		switch (ptr->token) {
		case EKWA_INFO:
			printf(INFO_BLOCK);
			break;
		}

		ptr = ptr->next;
	}
}