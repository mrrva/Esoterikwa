
#include "runtime.h"

void
ekwa_hex_buffer(unsigned char *);

void
ekwa_virtual_machine(struct ekwa_instruction *list)
{
	size_t buff_size = MAXBUFFER_LEN + sizeof(uint16_t);
	struct ekwa_instruction *ptr = list;
	unsigned char *buffer;
	size_t cur_line = 0;

	if (!list || list == NULL) {
		printf("[E]: Instrunctions struct is empty.\n");
		exit(1);
	}

	if (!(buffer = (unsigned char *)malloc(buff_size))) {
		printf("[E]: Can't allocate memory.\n");
		exit(1);
	}

	memset(buffer, 0x00, buff_size);
	ekwa_set_flags(list);

	while (ptr && ptr != NULL) {
		switch (ptr->token) {
		case EKWA_INFO:
			printf(INFO_BLOCK);
			break;

		case EKWA_SHOW:
			ekwa_token_show(ptr);
			break;

		case EKWA_VAR:
			ekwa_token_var(ptr);
			break;

		case EKWA_BUFF:
			ekwa_token_buffer(ptr, &buffer);
			break;

		case EKWA_WRT:
			ekwa_token_write(ptr, buffer);
			memset(buffer, 0x00, buff_size);
			break;

		case EKWA_JMP:
			ekwa_token_jump(&ptr);
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
ekwa_new_var(struct ekwa_var *new)
{
	if (!new || new == NULL) {
		printf("[W]: Can't add new var.\n");
		return;
	}

	new->next = ekwa_vars;
	ekwa_vars = new;
}

void
ekwa_exception(enum ekwa_tokens token, struct ekwa_var *var,
			bool fatal)
{
	char *name = ekwa_token_name(token), *var_name;
	uint16_t size;

	if ((!var || var == NULL) && fatal) {
		printf("\n[Exception]: Incorrect argument for "
				"token %s\n", name);
		exit(1);
	}
	else if ((!var || var == NULL) && !fatal) {
		printf("\n[Exception]: Incorrect argument for "
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
		printf("\n[Exception]: Incorrect argument for "
				"token %s\n", name);

		if (fatal) {
			exit(1);
		}
		return;
	}

	strncpy(var_name, var->name, size);
	printf("\n[Exception]: Token: %s Var: %s", name,
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

void
ekwa_set_flags(struct ekwa_instruction *list)
{
	size_t bsize = MAXBUFFER_LEN + sizeof(uint16_t);
	size_t i = 0, fsize = sizeof(struct ekwa_flag);
	struct ekwa_instruction *tmp = list;
	struct ekwa_flag *flag;

	if (!list || list == NULL) {
		printf("[E]: ekwa_get_flags args.\n");
		exit(1);
	}

	while (i++, tmp != NULL) {
		if (tmp->token != EKWA_FSET) {
			tmp = tmp->next;
			continue;
		}

		if (tmp->arg1[0] + tmp->arg1[1] == 0x00) {
			printf("[E]: Inccorect flag in %lu line.\n",
					i - 1);
			exit(1);
		}

		if (!tmp->next || tmp->next == NULL) {
			printf("[E]: Can't set flag.\n");
			exit(1);
		}

		flag = (struct ekwa_flag *)malloc(fsize);

		if (!flag) {
			printf("[E]: Can't allocate memory.\n");
			exit(1);
		}

		memcpy(flag->name, tmp->arg1, bsize);
		flag->next = ekwa_flags;
		flag->point = tmp;
		ekwa_flags = flag;

	#ifdef RUNTIME_DEBUG
		printf("[I]: New flag, line %lu.\n", i - 1);
	#endif
		tmp = tmp->next->next;
	}
}

struct ekwa_flag *
ekwa_get_flag(unsigned char *buffer)
{
	struct ekwa_flag *tmp = ekwa_flags;
	uint16_t len = 0;

	if (!buffer || buffer == NULL) {
		printf("[E]: ekwa_get_flag args.\n");
		exit(1);
	}

	memcpy(&len, buffer, sizeof(uint16_t));

	if (len == 0) {
		printf("[E]: Incorrect name of flag.\n");
		exit(1);
	}

	while (tmp != NULL) {
		if (memcmp(buffer + 2, tmp->name + 2,
					len) != 0) {
			tmp = tmp->next;
			continue;
		}

		return tmp;
	}
}