
#include "runtime.h"

void
ekwa_token_show(struct ekwa_instruction *line)
{
	struct ekwa_var *var;
	uint16_t size;
	char *value;

	if (!line || line == NULL) {
		printf("[E]: ekwa_token_show args.\n");
		exit(1);
	}

	var = ekwa_find_var(line->arg1);

	if (!var || var == NULL) {
		ekwa_exception(line->token, var, false);
		return;
	}

	memcpy(&size, var->value, sizeof(uint16_t));

	if (size == 0) {
		return;
	}

	if (!(value = (char *)malloc(size + 1))) {
		printf("[E]: Can't allocate memory.\n");
		exit(1);
	}

	strncpy(value, var->value + 2, size);
	printf("%s", value);
	free(value);
}

void
ekwa_token_var(struct ekwa_instruction *line)
{
	size_t copy_size = MAXBUFFER_LEN + sizeof(uint16_t);
	struct ekwa_var *var;

	if (!line || line == NULL) {
		printf("[E]: ekwa_token_var args.\n");
		exit(1);
	}

	if (line->arg1[0] + line->arg1[1] == 0x00) {
		ekwa_exception(line->token, var, true);
	}

	var = ekwa_find_var(line->arg1);

	if (var && var != NULL) {
	#ifdef RUNTIME_DEBUG
		printf("[I]: Var was updated.\n");
	#endif
		memcpy(var->value, line->arg2, copy_size);
		return;
	}

	var = (struct ekwa_var *)malloc(sizeof(struct ekwa_var));

	if (!var) {
		printf("[E]: Can't allocate memory.\n");
		exit(1);
	}

	memcpy(var->name, line->arg1,  copy_size);
	memcpy(var->value, line->arg2, copy_size);

	ekwa_new_var(var);
#ifdef RUNTIME_DEBUG
	printf("[I]: Var was added.\n");
#endif
}

void
ekwa_token_buffer(struct ekwa_instruction *line,
				unsigned char **buffer)
{
	size_t copy_size = MAXBUFFER_LEN + sizeof(uint16_t);
	struct ekwa_var *var;

	if (!line || line == NULL) {
		printf("[E]: ekwa_token_buffer args.\n");
		exit(1);
	}

	var = ekwa_find_var(line->arg1);

	if (!var || var == NULL) {
		ekwa_exception(line->token, var, true);
	}

	memcpy(*buffer, var->value, copy_size);
#ifdef RUNTIME_DEBUG
	printf("[I]: New value in buffer.\n");
#endif
}

void
ekwa_token_write(struct ekwa_instruction *line,
				unsigned char *buffer)
{
	size_t copy_size = MAXBUFFER_LEN + sizeof(uint16_t);
	struct ekwa_var *var;

	if (!line || line == NULL) {
		printf("[E]: ekwa_token_write args.\n");
		exit(1);
	}

	var = ekwa_find_var(line->arg1);

	if (!var || var == NULL) {
		ekwa_exception(line->token, var, true);
	}

	memcpy(var->value, buffer, copy_size);
#ifdef RUNTIME_DEBUG
	printf("[I]: Was written data from buffer.\n");
#endif
}

void
ekwa_token_jump(struct ekwa_instruction **line)
{
	struct ekwa_flag *flag;

	if (!(*line) || (*line) == NULL) {
		printf("[E]: ekwa_token_jump args.\n");
		exit(1);
	}

	flag = ekwa_get_flag((*line)->arg1);

	if (!flag || flag == NULL) {
		printf("[E]: Can't find flag.\n");
		exit(1);
	}

	*line = flag->point;
#ifdef RUNTIME_DEBUG
	printf("[I]: Jump to flag %s.\n",
			flag->name + 2);
#endif
}

void
ekwa_token_comparing(struct ekwa_instruction **line)
{
	struct ekwa_var *var_1, *var_2;
	uint16_t len1 = 0, len2 = 0;

	if (!(*line) || (*line) == NULL) {
		printf("[E]: ekwa_token_comparing args.\n");
		exit(1);
	}

	var_1 = ekwa_find_var((*line)->arg1);
	var_2 = ekwa_find_var((*line)->arg2);

	if (!var_1 || var_1 == NULL || !var_2
		|| var_2 == NULL) {
		printf("[E]: Incorrect comparing.\n");
		exit(1);
	}

	memcpy(&len1, var_1->value, sizeof(uint16_t));
	memcpy(&len2, var_2->value, sizeof(uint16_t));

	if (len1 == 0 || len2 == 0) {
		printf("[E]: Incorrect length of arg "
				"in ekwa_token_comparing.\n");
		exit(1);
	}

	if (len1 != len2) {
		if (!(*line)->next->next
			|| (*line)->next->next == NULL) {
			exit(1);
		}

		*line = (*line)->next;
		return;
	}

	if (memcmp(var_1->value + 2, var_2->value + 2,
				len1) != 0) {
		if (!(*line)->next->next
			|| (*line)->next->next == NULL) {
			exit(1);
		}
		*line = (*line)->next;
		return;
	}
}

char *
ekwa_token_name(enum ekwa_tokens token)
{
	char *name;

	if (token >= EKWA_END || token == 0x00) {
		return NULL;
	}

	switch (token) {
	case EKWA_VAR:
		name = "EKWA_VAR";
		break;

	case EKWA_BUFF:
		name = "EKWA_BUFF";
		break;

	case EKWA_ARG:
		name = "EKWA_ARG";
		break;

	case EKWA_CALL:
		name = "EKWA_CALL";
		break;

	case EKWA_JMP:
		name = "EKWA_JMP";
		break;

	case EKWA_FSET:
		name = "EKWA_FSET";
		break;

	case EKWA_WRT:
		name = "EKWA_WRT";
		break;

	case EKWA_CMP:
		name = "EKWA_CMP";
		break;

	case EKWA_IFS:
		name = "EKWA_IFS";
		break;

	case EKWA_IFB:
		name = "EKWA_IFB";
		break;

	case EKWA_INFO:
		name = "EKWA_INFO";
		break;

	case EKWA_SHOW:
		name = "EKWA_SHOW";
		break;

	case EKWA_RMV:
		name = "EKWA_RMV";
		break;

	default:
		name = "EKWA_UNDECLARED";
		break;
	}

	return name;
}