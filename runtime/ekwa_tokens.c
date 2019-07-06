
#include "runtime.h"

void
ekwa_token_show(struct ekwa_instruction *line)
{
	struct ekwa_var *var;
	uint16_t size;
	char *value;

	if (!line || line == NULL) {
		printf("\n[E]: ekwa_token_show args.\n");
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
		printf("\n[E]: Can't allocate memory.\n");
		exit(1);
	}

	strncpy(value, var->value + 2, size);
	printf("%.*s", size, value);
	free(value);
}

void
ekwa_token_var(struct ekwa_instruction *line)
{
	size_t copy_size = MAXBUFFER_LEN + sizeof(uint16_t);
	struct ekwa_var *var;

	if (!line || line == NULL) {
		printf("\n[E]: ekwa_token_var args.\n");
		exit(1);
	}

	if (line->arg1[0] + line->arg1[1] == 0x00) {
		ekwa_exception(line->token, var, true);
	}

	var = ekwa_find_var(line->arg1);

	if (var && var != NULL) {
	#ifdef RUNTIME_DEBUG
		printf("\n[I]: Var was updated.\n");
	#endif
		memcpy(var->value, line->arg2, copy_size);
		return;
	}

	var = (struct ekwa_var *)malloc(sizeof(struct ekwa_var));

	if (!var) {
		printf("\n[E]: Can't allocate memory.\n");
		exit(1);
	}

	memcpy(var->name, line->arg1,  copy_size);
	memcpy(var->value, line->arg2, copy_size);

	ekwa_new_var(var);
#ifdef RUNTIME_DEBUG
	printf("\n[I]: Var was added.\n");
#endif
}

void
ekwa_token_buffer(struct ekwa_instruction *line,
				unsigned char **buffer)
{
	size_t copy_size = MAXBUFFER_LEN + sizeof(uint16_t);
	struct ekwa_var *var;

	if (!line || line == NULL) {
		printf("\n[E]: ekwa_token_buffer args.\n");
		exit(1);
	}

	var = ekwa_find_var(line->arg1);

	if (!var || var == NULL) {
		ekwa_exception(line->token, var, true);
	}

	memcpy(*buffer, var->value, copy_size);
#ifdef RUNTIME_DEBUG
	printf("\n[I]: New value in buffer.\n");
#endif
}

void
ekwa_token_write(struct ekwa_instruction *line,
				unsigned char *buffer)
{
	size_t copy_size = MAXBUFFER_LEN + sizeof(uint16_t);
	struct ekwa_var *var;

	if (!line || line == NULL) {
		printf("\n[E]: ekwa_token_write args.\n");
		exit(1);
	}

	var = ekwa_find_var(line->arg1);

	if (!var || var == NULL) {
		ekwa_exception(line->token, var, true);
	}

	memcpy(var->value, buffer, copy_size);
#ifdef RUNTIME_DEBUG
	printf("\n[I]: Was written data from buffer.\n");
#endif
}

void
ekwa_token_jump(struct ekwa_instruction **line)
{
	struct ekwa_flag *flag;
	uint16_t size;

	if (!(*line) || (*line) == NULL) {
		printf("\n[E]: ekwa_token_jump args.\n");
		exit(1);
	}

	flag = ekwa_get_flag((*line)->arg1);

	if (!flag || flag == NULL) {
		printf("\n[E]: Can't find flag.\n");
		exit(1);
	}

	*line = flag->point;
#ifdef RUNTIME_DEBUG
	memcpy(&size, (*line)->arg1, sizeof(uint16_t));
	printf("\n[I]: Jump to flag %.*s.\n",
			size, flag->name + 2);
#endif
}

void
ekwa_token_comparing(struct ekwa_instruction **line)
{
	struct ekwa_var *var_1, *var_2;
	uint16_t len1 = 0, len2 = 0;

	if (!(*line) || (*line) == NULL) {
		printf("\n[E]: ekwa_token_comparing args.\n");
		exit(1);
	}

	var_1 = ekwa_find_var((*line)->arg1);
	var_2 = ekwa_find_var((*line)->arg2);

	if (!var_1 || var_1 == NULL || !var_2
		|| var_2 == NULL) {
		printf("\n[E]: Incorrect comparing.\n");
		exit(1);
	}

	memcpy(&len1, var_1->value, sizeof(uint16_t));
	memcpy(&len2, var_2->value, sizeof(uint16_t));

	if (len1 == 0 || len2 == 0) {
		printf("\n[E]: Incorrect length of arg "
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

void
ekwa_token_remove_var(struct ekwa_instruction *line)
{
	struct ekwa_var *tmp = ekwa_vars, *dat;
	uint16_t len1 = 0, len2 = 0;

	if (!line || line == NULL) {
		printf("\n[E]: ekwa_token_remove_var args.\n");
		exit(1);
	}

	memcpy(&len1, line->arg1, sizeof(uint16_t));

	if (len1 == 0) {
		printf("\n[E]: Incorrect name of var in "
				"ekwa_token_remove_var.\n");
		return;
	}

	memcpy(&len2, tmp->name, sizeof(uint16_t));

	if (len1 == len2
		&& memcmp(tmp->name + 2, line->arg1 + 2,
				len1) == 0) {
	#ifdef RUNTIME_DEBUG
		printf("\n[I]: Romoved %.*s var\n", len2,
				tmp->name + 2);
	#endif
		dat = ekwa_vars;
		ekwa_vars = ekwa_vars->next;
		free(dat);
		return;
	}

	while (tmp->next && tmp->next != NULL) {
		memcpy(&len2, tmp->next->name, sizeof(uint16_t));

		if (len1 != len2) {
			tmp = tmp->next;
			continue;
		}

		if (memcmp(tmp->next->name + 2, line->arg1 + 2,
					len1) == 0) {
		#ifdef RUNTIME_DEBUG
			printf("\n[I]: Removed %.*s var\n", len2,
					tmp->next->name + 2);
		#endif
			dat = tmp->next;
			tmp->next = tmp->next->next;
			free(dat);
			return;
		}

		tmp = tmp->next;
	}
}

void
ekwa_token_add_arg(struct ekwa_instruction *line)
{
	size_t buff_size = MAXBUFFER_LEN + sizeof(uint16_t);
	size_t size = sizeof(struct ekwa_arg);
	struct ekwa_arg *arg, *tmp;
	struct ekwa_var *var;

	if (!line || line == NULL) {
		printf("\n[E]: ekwa_token_add_arg args.\n");
		exit(1);
	}

	var = ekwa_find_var(line->arg1);

	if (!var || var == NULL) {
		printf("\n[E]: Can't find var in list.\n");
		return;
	}

	arg = (struct ekwa_arg *)malloc(size);

	if (!arg) {
		printf("\n[E]: Can't allocate memory.\n");
		exit(1);
	}

	arg->next = NULL;
	memcpy(arg->value, var->value, buff_size);

	if (!ekwa_args || ekwa_args == NULL) {
	#ifdef RUNTIME_DEBUG
		printf("\n[I]: Added new argument.\n");
	#endif
		ekwa_args = arg;
		return;
	}

	tmp = ekwa_args;

	while (tmp->next && tmp->next != NULL) {
		tmp = tmp->next;
	}

	tmp->next = arg;
#ifdef RUNTIME_DEBUG
	printf("\n[I]: Added new argument.\n");
#endif
	/*arg->next = NULL;
	arg->var = var;

	if (!ekwa_args || ekwa_args == NULL) {
	#ifdef RUNTIME_DEBUG
		printf("\n[I]: Added new argument.\n");
	#endif
		ekwa_args = arg;
		return;
	}

	tmp = ekwa_args;

	while (tmp->next && tmp->next != NULL) {
		tmp = tmp->next;
	}

	tmp->next = arg;
#ifdef RUNTIME_DEBUG
	printf("\n[I]: Added new argument.\n");
#endif*/
}

bool
ekwa_call_fromlib(char *ret, char *name)
{
	typedef void (*lfunc)();

	void *handle = NULL;
	char *lname, *fname;
	lfunc func = NULL;

	if (!ret || !name) {
		printf("\n[E]: ekwa_call_fromlib args.\n");
		exit(1);
	}

	fname = (char *)malloc(strlen(ret) + 12);
	lname = (char *)malloc(ret - name  + 10);

	if (!fname || !lname) {
		printf("\n[E]: Can't allocate memory.\n");
		exit(1);
	}

	sprintf(lname, "./ekwa%.*s.so", (int)(ret - name),
			name);
	sprintf(fname, "ekwa_%s", ret + 1);

	handle = dlopen(lname, RTLD_NOW | RTLD_GLOBAL);

	if (handle == NULL) {
		printf("\n[E]: Can't open %s : %s\n", lname,
				dlerror());
		exit(1);
	}

	func = dlsym(handle, fname);

	if (func == NULL) {
		printf("\n[E]: Can't find %s in %s.\n",
				fname, lname);
		exit(1);
	}

	func(ekwa_args);

	free(lname);
	free(fname);
}

void
ekwa_token_call(struct ekwa_instruction *line)
{
	struct ekwa_functions *tmp = ekwa_funcs;
	uint16_t len = 0;
	char *name, *ret;

	if (!line || line == NULL) {
		printf("\n[E]: ekwa_token_call args.\n");
		exit(1);
	}

	memcpy(&len, line->arg1, sizeof(uint16_t));

	if (len == 0 || len > MAXBUFFER_LEN) {
		printf("\n[E]: Incorrect call.\n");
		exit(1);
	}

	name = (char *)malloc(len + 1);

	if (!name) {
		printf("\n[E]: Can't allocate memory.\n");
		exit(1);
	}

	memcpy(name, line->arg1 + 2, len);
	name[len + 1] = 0x00;

#ifdef RUNTIME_DEBUG
	printf("\n[I]: Trying call %s\n", name);
#endif

	if ((ret = strchr(name, '.'))) {
		ekwa_call_fromlib(ret, name);
		return;
	}

	/**********STD LIB HERE *********/
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

	case EKWA_TYPE:
		name = "EKWA_TYPE";
		break;

	default:
		name = "EKWA_UNDECLARED";
		break;
	}

	return name;
}