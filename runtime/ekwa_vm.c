
#include "runtime.h"

void
ekwa_hex_buffer(unsigned char *);

void
ekwa_arguments_clear(void);

void
ekwa_virtual_machine(struct ekwa_instruction *list)
{
	struct ekwa_instruction *ptr = list;
	struct ekwa_var buffer;
	size_t line = 0;

	if (!list || list == NULL) {
		printf("\n[E]: Instrunctions struct is empty.\n");
		exit(1);
	}

	memset(&buffer, 0x00, sizeof(buffer));
	ekwa_set_flags(list);

	while (ptr && ptr != NULL) {
	#ifdef RUNTIME_DEBUG
		printf("\n[I]: Current line: %ld.\n", line);
		ekwa_show_all_vars();
		line++;
	#endif

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

		case EKWA_VAL:
			ekwa_token_set_value(ptr);
			break;

		case EKWA_BUFF:
			ekwa_token_buffer(ptr, &buffer);
			break;

		case EKWA_WRT:
			ekwa_token_write(ptr, &buffer);
			memset(&buffer, 0x00, sizeof(buffer));
			break;

		case EKWA_JMP:
			ekwa_token_jump(&ptr);
			break;

		case EKWA_RMV:
			ekwa_token_remove_var(ptr);
			break;

		case EKWA_ARG:
			ekwa_token_add_arg(ptr);
			break;

		case EKWA_CALL:
			ekwa_token_call(ptr, &buffer);
			ekwa_arguments_clear();
			break;

		case EKWA_IFE:
			ekwa_token_equal(&ptr, true);
			break;

		case EKWA_IFS:
			ekwa_token_ifsmaller(&ptr);
			break;

		case EKWA_IFB:
			ekwa_token_ifbigger(&ptr);
			break;

		case EKWA_IFNE:
			ekwa_token_equal(&ptr, false);
			break;

		case EKWA_ADD:
		case EKWA_SUB:
		case EKWA_DIV:
		case EKWA_MOD:
		case EKWA_MUL:
			ekwa_token_math(ptr, &buffer, ptr->token);
			break;

		case EKWA_CAT:
			ekwa_token_concat(ptr);
			break;

		case EKWA_OPT:
			ekwa_token_set_opt(ptr);
			break;

		case EKWA_EXIT:
			exit(0);
		}

		ptr = ptr->next;
	}
}

struct ekwa_var *
ekwa_find_var(char *name)
{
	struct ekwa_var *tmp = ekwa_vars;
	uint16_t size = strlen(name);

	if (!name || name == NULL || size == 0
		|| size > MAXBUFFER_LEN) {
		return NULL;
	}

	while (tmp != NULL) {
		if (strcmp(name, tmp->name) == 0) {
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
		printf("\n[W]: Can't add new var.\n");
		return;
	}

	new->next = ekwa_vars;
	ekwa_vars = new;
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
	size_t i = 0, fsize = sizeof(struct ekwa_flag);
	struct ekwa_instruction *tmp = list;
	struct ekwa_buffer arg1;
	struct ekwa_flag *flag;

	if (!list || list == NULL) {
		printf("\n[E]: ekwa_get_flags args.\n");
		exit(1);
	}

	while (i++, tmp != NULL) {
		if (tmp->token != EKWA_FSET) {
			tmp = tmp->next;
			continue;
		}

		arg1 = ekwa_decode_buffer(tmp->arg1);

		if (arg1.length > MAXBUFFER_LEN
			|| arg1.length == 0) {
			printf("\n[E]: Inccorect flag in %lu line"
					".\n", i - 1);
			exit(1);
		}

		flag = (struct ekwa_flag *)malloc(fsize);

		if (!flag) {
			printf("\n[E]: Can't allocate memory.\n");
			exit(1);
		}

		strncpy(flag->name, arg1.data, arg1.length);
		flag->next = ekwa_flags;
		flag->point = tmp;
		ekwa_flags = flag;

	#ifdef RUNTIME_DEBUG
		printf("\n[I]: New flag %s. line: %ld.\n", 
				flag->name, i - 1);
	#endif
		tmp = tmp->next->next;
	}
}

struct ekwa_flag *
ekwa_get_flag(char *name)
{
	struct ekwa_flag *tmp = ekwa_flags;
	size_t len = strlen(name);

	if (!name || name == NULL || len == 0
		|| len > MAXBUFFER_LEN) {
		printf("\n[E]: ekwa_get_flag args.\n");
		exit(1);
	}

	while (tmp != NULL) {
		if (strcmp(name, tmp->name) != 0) {
			tmp = tmp->next;
			continue;
		}

		return tmp;
	}

	return NULL;
}

void
ekwa_arguments_clear(void)
{
	struct ekwa_arg *ptr = ekwa_args, *tmp;

	if (!tmp || tmp == NULL) {
		return;
	}

	while (ptr && ptr != NULL) {
		tmp = ptr->next;
		free(ptr);
		ptr = tmp;
	}

	ekwa_args = NULL;
}

struct ekwa_buffer
ekwa_decode_buffer(unsigned char *buffer)
{
	struct ekwa_buffer ret;

	if (!buffer || buffer == NULL) {
		printf("\n[E]: Can't decode buffer.\n");
		exit(1);
	}

	memcpy(&ret.length, buffer, sizeof(uint16_t));
	memset(ret.data, 0x00, MAXBUFFER_LEN + 1);
	memcpy(ret.data, buffer + 2, ret.length);

	if (ret.length > MAXBUFFER_LEN
		|| ret.length == 0) {
		printf("\n[E]: Incorrect buffer size.\n");
		exit(1);
	}

	return ret;
}

enum ekwa_types
ekwa_detect_type(unsigned char *buffer)
{
	uint16_t len = 0;

	if (!buffer || buffer == NULL) {
		return EKWA_BYTES;
	}

	memcpy(&len, buffer, sizeof(uint16_t));

	if (len != 1) {
		return EKWA_BYTES;
	}

	if (buffer[2] > EKWA_CUSTOM) {
		return EKWA_BYTES;
	}

	return (enum ekwa_types)buffer[2];
}

int
ekwa_buffer_to_int(unsigned char *buffer)
{
	uint16_t len = 0;
	int value = 0;

	if (!buffer || buffer == NULL) {
		return 0;
	}

	memcpy(&len, buffer, sizeof(uint16_t));

	if (len == 0) {
		return 0;
	}

	memcpy(&value, buffer + 2, len);
	return value;
}

float
ekwa_buffer_to_float(unsigned char *buffer)
{
	uint16_t len = 0;
	float value = 0;

	if (!buffer || buffer == NULL) {
		return 0;
	}

	memcpy(&len, buffer, sizeof(uint16_t));

	if (len == 0) {
		return 0;
	}

	memcpy(&value, buffer + 2, len);
	return value;
}

void
ekwa_set_option(char *name, unsigned char *value)
{
	size_t b_size = MAXBUFFER_LEN + sizeof(uint16_t);
	struct ekwa_option *opt = NULL;
	size_t st_size = sizeof(struct ekwa_option);
	uint16_t len = 0;

	if (!name || name == NULL || !value
		|| value == NULL) {
		printf("\n[E]: Incorrect args in ekwa_set"
			"_option.\n");
		exit(1);
	}

	memcpy(&len, value, sizeof(uint16_t));

	if (len > MAXBUFFER_LEN || len == 0) {
		printf("\n[E]: Incorrect args in ekwa_set"
			"_option.\n");
		exit(1);
	}

#ifdef RUNTIME_DEBUG
	printf("\n[I] New value for %s option.\n", name);
#endif
	if ((opt = ekwa_find_option(name)) != NULL) {
		memcpy(opt->value, value, b_size);
		return;
	}

	opt = (struct ekwa_option *)malloc(st_size);

	if (!opt) {
		printf("\n[E]: Can't allocate memory.\n");
		exit(1);
	}

	memcpy(opt->value, value, b_size);
	strcpy(opt->name, name);

	opt->next = ekwa_opts;
	ekwa_opts = opt;
}

struct ekwa_option *
ekwa_find_option(char *name)
{
	struct ekwa_option *tmp = ekwa_opts;

	while (tmp && tmp != NULL) {
		if (strcmp(name, tmp->name) == 0) {
			return tmp;
		}

		tmp = tmp->next;
	}

	return NULL;
}

void
ekwa_set_default_opts(void)
{
	size_t b_size = MAXBUFFER_LEN + sizeof(uint16_t);
	unsigned char buff[b_size];
	uint16_t len = 2;

	memcpy(buff, &len, sizeof(uint16_t));
	memcpy(buff + 2, "./", 2);

	ekwa_set_option("libs_path", buff);
}

void
ekwa_show_all_vars(void)
{
	struct ekwa_var *var = ekwa_vars;
	struct ekwa_buffer buff;

	while (var && var != NULL) {
		printf("%s -> ", var->name);
		var = var->next;
	}

	printf("\n");
}