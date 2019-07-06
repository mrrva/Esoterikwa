
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#define BUFFSIZE 4000 + sizeof(uint16_t)

struct ekwa_arg {
	unsigned char value[BUFFSIZE];
	struct ekwa_arg *next;
};

void
ekwa_test(void *args)
{
	struct ekwa_arg *ekwa_args;
	uint16_t len = 0; 
	char *str;

	if (!args || args == NULL) {
		printf("\nekwa_test: Empty args.\n");
		return;
	}

	ekwa_args = (struct ekwa_arg *)args;

	memcpy(&len, ekwa_args->value, sizeof(uint16_t));
	str = (char *)malloc(len + 1);

	if (!str) {
		printf("\n[E]: Can't allocate memory\n");
		return;
	}

	memcpy(str, ekwa_args->value + 2, len);
	printf("ekwa_test: First arg is %.*s\n", len,
			str);

	free(str);
}