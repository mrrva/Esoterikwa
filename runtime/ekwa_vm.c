
#include "runtime.h"

void
ekwa_hex_buffer(unsigned char *);

void
ekwa_virtual_machine(struct ekwa_instruction *list)
{
	size_t buff_size = MAXBUFFER_LEN + sizeof(uint16_t);
	struct ekwa_instruction *ptr = list;
	unsigned char buffer[buff_size];
	size_t cur_line = 0;

	if (!list || list == NULL) {
		printf("[E]: Instrunctions struct is empty.\n");
		exit(1);
	}

	memset(buffer, 0x00, buff_size);

	while (ptr && ptr != NULL) {
		switch (ptr->token) {
		case EKWA_INFO:
			printf(INFO_BLOCK);
			break;

		case EKWA_SHOW:
			ekwa_token_show(ptr);
			break;
		}

		ptr = ptr->next;
		cur_line++;
	}
}

struct ekwa_var *
ekwa_find_var(unsigned char *name)
{
	struct ekwa_var *tmp = ekwa_vars;
	uint16_t size;

	if (!name || name == NULL) {
		return NULL;
	}

	memcpy(&size, name, sizeof(uint16_t));

	while (tmp != NULL) {
		if (memcmp(name + 2, tmp->name + 2,
					size) == 0) {
			return tmp;
		}

		tmp = tmp->next;
	}

	return NULL;
}

void
ekwa_exception(enum ekwa_tokens token, struct ekwa_var *var,
			bool fatal)
{
	char *name = ekwa_token_name(token), *var_name;
	uint16_t size;

	if ((!var || var == NULL) && fatal) {
		printf("[Exception]: Incorrect argument for "
				"token %s\n", name);
		exit(1);
	}
	else if ((!var || var == NULL) && !fatal) {
		printf("[Exception]: Incorrect argument for "
				"token %s\n", name);
		return;
	}

	memcpy(&size, var->name, sizeof(uint16_t));
	var_name = (char *)malloc(size + 1);

	if (!var_name) {
		printf("[E]: Can't allocate memory.\n");
		exit(1);
	}

	if (size == 0 || size > MAXBUFFER_LEN) {
		printf("[Exception]: Incorrect argument for "
				"token %s\n", name);

		if (fatal) {
			exit(1);
		}
		return;
	}

	strncpy(var_name, var->name, size);
	printf("[Exception]: Token: %s Var: %s", name,
			var_name);

	printf("[Var dump]:\n");
	ekwa_hex_buffer(var->value);

	if (fatal) {
		exit(1);
	}
}

void
ekwa_hex_buffer(unsigned char *buff)
{
	size_t line = 1, max = MAXBUFFER_LEN;
	uint16_t size;

	memcpy(&size, buff, sizeof(uint16_t));
	max = (size > max) ? max : size; 

	for (size_t i = 0; i < max; i++) {
		printf("0x%.2x ", buff[i + 2]);

		if (line - 10 == 1) {
			printf("\n");
			line = 1;
			continue;
		}

		line++;
	}

	printf("\n");
}