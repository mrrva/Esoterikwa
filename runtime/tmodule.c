
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#define BUFFSIZE 4000 + sizeof(uint16_t)

enum ekwa_types {
	EKWA_BYTES	= 0x00,
	EKWA_INT	= 0x01,
	EKWA_FLOAT	= 0x02,
	EKWA_CUSTOM	= 0x03
};

struct ekwa_arg {
	unsigned char value[BUFFSIZE];
	enum ekwa_types type;
	struct ekwa_arg *next;
};

void *
ekwa_test(void *args)
{
	size_t size = sizeof(struct ekwa_arg);
	struct ekwa_arg *ekwa_args;
	struct ekwa_arg *new;
	uint16_t len = 0;
	char *str;

	if (!args || args == NULL) {
		printf("\nekwa_test: Empty args.\n");
		return NULL;
	}

	ekwa_args = (struct ekwa_arg *)args;
	memcpy(&len, ekwa_args->value, sizeof(uint16_t));

	if (len == 0) {
		printf("\nnekwa_test: Invalid len of first "
				"argument.\n");
		exit(1);
	}

	str = (char *)malloc(len + 1);
	memcpy(str, ekwa_args->value + 2, len);
	str[len] = 0x00;

	printf("\nekwa_test: %s\n", str);

	if (!(new = (struct ekwa_arg *)malloc(size))) {
		printf("\n[E]: ekwa_test - can't allocate "
				"memory.\n");
		exit(1);
	}

	len = 13;

	memcpy(new->value + 2, "Hello world!", len);
	memcpy(new->value, &len, sizeof(uint16_t));
	new->type = EKWA_BYTES;

	return (void *)new;
}