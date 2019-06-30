
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