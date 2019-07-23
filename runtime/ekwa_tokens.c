
#include "runtime.h"

void
ekwa_token_show(struct ekwa_instruction *line)
{
	struct ekwa_buffer arg1, print;
	struct ekwa_var *var;
	char *value;
	float fval;
	int ival;

	if (!line || line == NULL) {
		printf("\n[E]: ekwa_token_show args.\n");
		exit(1);
	}

	arg1 = ekwa_decode_buffer(line->arg1);
	var = ekwa_find_var(arg1.data);

	if (!var || var == NULL) {
		printf("\n[W]: Incorrect var %s for ekwa_t"
				"oken_show.\n", arg1.data);
		return;
	}

	if (var->type == EKWA_INT) {
		ival = ekwa_buffer_to_int(var->value);
		printf("%d\n", ival);
		return;
	}

	if (var->type == EKWA_FLOAT) {
		fval = ekwa_buffer_to_float(var->value);
		printf("%f\n", fval);
		return;
	}

	print = ekwa_decode_buffer(var->value);

	if (!(value = (char *)malloc(print.length + 2))) {
		printf("\n[E]: Can't allocate memory.\n");
		exit(1);
	}

	strncpy(value, print.data, print.length);
	value[print.length] = 0x00;
	printf("%s\n", value);
	free(value);
}

void
ekwa_token_var(struct ekwa_instruction *line)
{
	struct ekwa_buffer arg1;
	struct ekwa_var *var;

	if (!line || line == NULL) {
		printf("\n[E]: ekwa_token_var args.\n");
		exit(1);
	}

	arg1 = ekwa_decode_buffer(line->arg1);
	var = ekwa_find_var(arg1.data);

	if (var && var != NULL) {
		printf("\n[W]: Var %s already exists.\n", arg1.data);
		return;
	}

	var = (struct ekwa_var *)malloc(sizeof(struct ekwa_var));

	if (!var) {
		printf("\n[E]: Can't allocate memory.\n");
		exit(1);
	}

	memset(var->name, 0x00, MAXBUFFER_LEN + 1);
	strncpy(var->name, arg1.data, arg1.length);
	var->name[arg1.length] = 0x00;

	var->type = ekwa_detect_type(line->arg2);
	memset(var->value, 0x00, 3);
	ekwa_new_var(var);

#ifdef RUNTIME_DEBUG
	printf("\n[I]: Var %s was added, type: %x.\n",
			var->name, var->type);
#endif
}

void
ekwa_token_set_value(struct ekwa_instruction *line)
{
	size_t size = MAXBUFFER_LEN + sizeof(uint16_t);
	struct ekwa_buffer arg1;
	struct ekwa_var *var;

	if (!line || line == NULL) {
		printf("\n[E]: ekwa_token_set_value args.\n");
		exit(1);
	}

	arg1 = ekwa_decode_buffer(line->arg1);
	var = ekwa_find_var(arg1.data);

	if (!var || var == NULL) {
		printf("\n[W]: Can't find var in ekwa_token_"
				"set_value.\n");
		return;
	}

	memcpy(var->value, line->arg2, size);
}

void
ekwa_token_buffer(struct ekwa_instruction *line,
				struct ekwa_var *buffer)
{
	size_t copy_size = sizeof(struct ekwa_var);
	struct ekwa_buffer arg1;
	struct ekwa_var *var;

	if (!line || line == NULL || !buffer) {
		printf("\n[E]: ekwa_token_buffer args.\n");
		exit(1);
	}

	arg1 = ekwa_decode_buffer(line->arg1);
	var = ekwa_find_var(arg1.data);

	if (!var || var == NULL) {
		printf("\n[W]: Can't find var in ekwa_token"
				"_buffer: %s.\n", arg1.data);
		return;
	}

	memcpy(buffer, var, copy_size);
#ifdef RUNTIME_DEBUG
	printf("\n[I]: New value in buffer.\n");
#endif
}

void
ekwa_token_write(struct ekwa_instruction *line,
				struct ekwa_var *buffer)
{
	size_t copy_size = sizeof(struct ekwa_var);
	struct ekwa_buffer arg1;
	struct ekwa_var *var;

	if (!line || line == NULL || !buffer) {
		printf("\n[E]: ekwa_token_write args.\n");
		exit(1);
	}

	arg1 = ekwa_decode_buffer(line->arg1);
	var = ekwa_find_var(arg1.data);

	if (!var || var == NULL) {
		printf("\n[W]: Can't find var in ekwa_token"
				"_write.\n");
		return;
	}

	strcpy(buffer->name, var->name);
	buffer->next = var->next;

	memcpy(var, buffer, copy_size);
#ifdef RUNTIME_DEBUG
	printf("\n[I]: Was written data from buffer "
		"to %s.\n", arg1.data);
#endif
}

void
ekwa_token_jump(struct ekwa_instruction **line)
{
	struct ekwa_buffer arg1;
	struct ekwa_flag *flag;
	uint16_t size;

	if (!(*line) || (*line) == NULL) {
		printf("\n[E]: ekwa_token_jump args.\n");
		exit(1);
	}

	arg1 = ekwa_decode_buffer((*line)->arg1);
	flag = ekwa_get_flag(arg1.data);

	if (!flag || flag == NULL) {
		printf("\n[E]: Can't find flag %s.\n",
				arg1.data);
		exit(1);
	}

	*line = flag->point;
#ifdef RUNTIME_DEBUG
	printf("\n[I]: Jump to flag %s.\n",
			arg1.data);
#endif
}

void
ekwa_token_remove_var(struct ekwa_instruction *line)
{
	struct ekwa_var *tmp = ekwa_vars, *dat;
	struct ekwa_buffer arg1, arg2;

	if (!line || line == NULL) {
		printf("\n[E]: ekwa_token_remove_var args.\n");
		exit(1);
	}

	arg1 = ekwa_decode_buffer(line->arg1);

	if (strcmp(arg1.data, tmp->name) == 0) {
	#ifdef RUNTIME_DEBUG
		printf("\n[I]: %s var is removed.\n", tmp->name);
	#endif
		dat = ekwa_vars;
		ekwa_vars = ekwa_vars->next;
		free(dat);
		return;
	}

	while (tmp->next && tmp->next != NULL) {
		if (strcmp(tmp->next->name, arg1.data) == 0) {
		#ifdef RUNTIME_DEBUG
			printf("\n[I]: %s var is removed.\n", arg1.data);
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
	size_t copy_buff = MAXBUFFER_LEN + sizeof(uint16_t);
	size_t size = sizeof(struct ekwa_arg);
	struct ekwa_arg *arg, *tmp;
	struct ekwa_buffer arg1;
	struct ekwa_var *var;

	if (!line || line == NULL) {
		printf("\n[E]: ekwa_token_add_arg args.\n");
		exit(1);
	}

	arg1 = ekwa_decode_buffer(line->arg1);
	var = ekwa_find_var(arg1.data);

	if (!var || var == NULL) {
		printf("\n[E]: Can't find var in list.\n");
		return;
	}

	arg = (struct ekwa_arg *)malloc(size);

	if (!arg) {
		printf("\n[E]: Can't allocate memory.\n");
		exit(1);
	}

	memcpy(arg->value, var->value, copy_buff);
	arg->type = var->type;
	arg->next = NULL;

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
}

bool
ekwa_call_fromlib(char *ret, char *name,
				struct ekwa_var *buffer)
{
	typedef void *(*lfunc)();

	size_t buff_size = MAXBUFFER_LEN + sizeof(uint16_t);
	struct ekwa_var *ret_val = NULL;
	void *handle = NULL;
	char *lname, *fname;
	lfunc func = NULL;

	if (!ret || !name || !buffer) {
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

	ret_val = (struct ekwa_var *)func(ekwa_args);

	if (ret_val && ret_val != NULL) {
		memcpy(buffer, ret_val, buff_size);
		free(ret_val);
	}

	free(lname);
	free(fname);
}

void
ekwa_token_call(struct ekwa_instruction *line,
				struct ekwa_var *buffer)
{
	struct ekwa_buffer arg1;
	char *ret;

	if (!line || line == NULL || !buffer) {
		printf("\n[E]: ekwa_token_call args.\n");
		exit(1);
	}

	arg1 = ekwa_decode_buffer(line->arg1);

#ifdef RUNTIME_DEBUG
	printf("\n[I]: Trying to call %s.\n", arg1.data);
#endif

	if ((ret = strchr(arg1.data, '.'))) {
		ekwa_call_fromlib(ret, arg1.data, buffer);
		return;
	}

	/*************** STD LIB ***************/
}

void
ekwa_token_equal(struct ekwa_instruction **line, bool eq)
{
	struct ekwa_buffer arg1, arg2;
	struct ekwa_var *var1, *var2;
	uint16_t len1, len2;
	float fval1, fval2;
	int ival1, ival2;

	if (!(*line) || (*line) == NULL) {
		printf("\n[E]: ekwa_token_equal args.\n");
		exit(1);
	}

	arg1 = ekwa_decode_buffer((*line)->arg1);
	arg2 = ekwa_decode_buffer((*line)->arg2);

	var1 = ekwa_find_var(arg1.data);
	var2 = ekwa_find_var(arg2.data);

	if (!var1 || var2 == NULL || !var1 || var2 == NULL) {
		printf("\n[E]: Can't select needed vars.\n");
		exit(1);
	}

	if (var1->type != var2->type) {
		printf("\n[E]: Can't compare not same types.\n");
		exit(1);
	}

	switch (var1->type) {
	case EKWA_INT:
		ival1 = ekwa_buffer_to_int(var1->value);
		ival2 = ekwa_buffer_to_int(var2->value);

		if (ival1 != ival2) {
			if (!eq) {
				return;
			}

			if ((*line)->next->next == NULL
				|| !(*line)->next->next) {
				exit(1);
			}
			*line = (*line)->next;
		}
		if (!eq) {
			if ((*line)->next->next == NULL
				|| !(*line)->next->next) {
				exit(1);
			}
		}

		return;

	case EKWA_FLOAT:
		fval1 = ekwa_buffer_to_float(var1->value);
		fval2 = ekwa_buffer_to_float(var2->value);

		if (fval1 != fval2) {
			if (!eq) {
				return;
			}

			if ((*line)->next->next == NULL
				|| !(*line)->next->next) {
				exit(1);
			}
			*line = (*line)->next;
		}
		if (!eq) {
			if ((*line)->next->next == NULL
				|| !(*line)->next->next) {
				exit(1);
			}
		}

		return;

	default:
		break;
	}

	memcpy(&len1, var1->value, sizeof(uint16_t));
	memcpy(&len2, var2->value, sizeof(uint16_t));

	if (len1 != len2) {
		if (!eq) {
			return;
		}
		if (!(*line)->next->next
			|| (*line)->next->next == NULL) {
			exit(1);
		}
		*line = (*line)->next;
		return;
	}

	if (memcmp(var1->value + 2, var2->value + 2,
				len1) != 0) {
		if (!eq) {
			return;
		}
		if (!(*line)->next->next
			|| (*line)->next->next == NULL) {
			exit(1);
		}
		*line = (*line)->next;
		return;
	}

	if (!eq) {
		if (!(*line)->next->next
			|| (*line)->next->next == NULL) {
			exit(1);
		}
	}
}

void
ekwa_token_ifsmaller(struct ekwa_instruction **line)
{
	struct ekwa_buffer arg1, arg2;
	struct ekwa_var *var1, *var2;
	float fval1, fval2;
	int ival1, ival2;

	if (!(*line) || (*line) == NULL) {
		printf("\n[E]: ekwa_token_ifsmaller args.\n");
		exit(1);
	}

	arg1 = ekwa_decode_buffer((*line)->arg1);
	arg2 = ekwa_decode_buffer((*line)->arg2);

	var1 = ekwa_find_var(arg1.data);
	var2 = ekwa_find_var(arg2.data);

	if (var1->type != EKWA_FLOAT
		&& var1->type != EKWA_INT) {
		printf("\n[E]: Incorrect type for ekwa_token"
				"_ifsmaller.\n");
		exit(1);
	}

	if (var1->type != var2->type) {
		printf("\n[E]: Incorrect type for ekwa_token"
				"_ifsmaller.\n");
		exit(1);
	}

	switch (var1->type) {
	case EKWA_INT:
		ival1 = ekwa_buffer_to_int(var1->value);
		ival2 = ekwa_buffer_to_int(var2->value);

		if (ival1 >= ival2) {
			if ((*line)->next->next == NULL
				|| !(*line)->next->next) {
				exit(1);
			}
			*line = (*line)->next;
		}
		return;

	case EKWA_FLOAT:
		fval1 = ekwa_buffer_to_float(var1->value);
		fval2 = ekwa_buffer_to_float(var2->value);

		if (fval1 >= fval2) {
			if ((*line)->next->next == NULL
				|| !(*line)->next->next) {
				exit(1);
			}
			*line = (*line)->next;
		}
		return;

	default:
		return;
	}
}

void
ekwa_token_ifbigger(struct ekwa_instruction **line)
{
	struct ekwa_buffer arg1, arg2;
	struct ekwa_var *var1, *var2;
	float fval1, fval2;
	int ival1, ival2;

	if (!(*line) || (*line) == NULL) {
		printf("\n[E]: ekwa_token_ifsmaller args.\n");
		exit(1);
	}

	arg1 = ekwa_decode_buffer((*line)->arg1);
	arg2 = ekwa_decode_buffer((*line)->arg2);

	var1 = ekwa_find_var(arg1.data);
	var2 = ekwa_find_var(arg2.data);

	if (var1->type != EKWA_FLOAT
		&& var1->type != EKWA_INT) {
		printf("\n[E]: Incorrect type for ekwa_token"
				"_ifbigger.\n");
		exit(1);
	}

	if (var1->type != var2->type) {
		printf("\n[E]: Incorrect type for ekwa_token"
				"_ifbigger.\n");
		exit(1);
	}

	switch (var1->type) {
	case EKWA_INT:
		ival1 = ekwa_buffer_to_int(var1->value);
		ival2 = ekwa_buffer_to_int(var2->value);

		if (ival1 <= ival2) {
			if ((*line)->next->next == NULL
				|| !(*line)->next->next) {
				exit(1);
			}
			*line = (*line)->next;
		}
		return;

	case EKWA_FLOAT:
		fval1 = ekwa_buffer_to_float(var1->value);
		fval2 = ekwa_buffer_to_float(var2->value);

		if (fval1 <= fval2) {
			if ((*line)->next->next == NULL
				|| !(*line)->next->next) {
				exit(1);
			}
			*line = (*line)->next;
		}
		return;

	default:
		return;
	}
}

void
ekwa_token_math(struct ekwa_instruction *line,
				struct ekwa_var *buffer,
				enum ekwa_tokens type)
{
	struct ekwa_var *var1, *var2;
	struct ekwa_buffer arg1, arg2;
	float fval1, fval2, fres;
	int ival1, ival2, ires;
	uint16_t len = 0;

	if (!line || line == NULL || !buffer) {
		printf("\n[E]: ekwa_token_add args.\n");
		exit(1);
	}

	arg1 = ekwa_decode_buffer(line->arg1);
	arg2 = ekwa_decode_buffer(line->arg2);

	var1 = ekwa_find_var(arg1.data);
	var2 = ekwa_find_var(arg2.data);

	if (!var1 || !var2 || var1 == NULL 
		|| var2 == NULL) {
		printf("\n[E]: Can't find vars in ekwa"
				"_token_add.\n");
		return;
	}

	if (var1->type != EKWA_FLOAT
		&& var1->type != EKWA_INT) {
		printf("\n[E]: Incorrect type for ekwa_token"
				"_add.\n");
		return;
	}

	if (var1->type != var2->type) {
		printf("\n[E]: Incorrect type for ekwa_token"
				"_add.\n");
		return;
	}

	switch (var1->type) {
	case EKWA_INT:
		ival1 = ekwa_buffer_to_int(var1->value);
		ival2 = ekwa_buffer_to_int(var2->value);

		if (type == 0x10) {
			ires = ival1 + ival2;
		}
		if (type == 0x11) {
			ires = ival1 - ival2;
		}
		if (type == 0x14) {
			ires = ival1 * ival2;
		}
		if (type == 0x12) {
			ires = ival1 / ival2;
		}
		if (type == 0x13) {
			ires = ival1 % ival2;
		}

		len = sizeof(ires);

		memcpy(buffer->value, &len, sizeof(uint16_t));
		memcpy(buffer->value + 2, &ires, len);
		buffer->type = EKWA_INT;
		return;

	case EKWA_FLOAT:
		fval1 = ekwa_buffer_to_float(var1->value);
		fval2 = ekwa_buffer_to_float(var2->value);

		if (type == 0x10) {
			fres = fval1 + fval2;
		}
		if (type == 0x11) {
			fres = fval1 - fval2;
		}
		if (type == 0x14) {
			fres = fval1 * fval2;
		}
		if (type == 0x12 || type == 0x13) {
			fres = fval1 / fval2;
		}
		len = sizeof(fres);

		memcpy(buffer->value, &len, sizeof(uint16_t));
		memcpy(buffer->value + 2, &fres, len);
		buffer->type = EKWA_FLOAT;
		return;

	default:
		return;
	}
}

void
ekwa_token_concat(struct ekwa_instruction *line)
{
	struct ekwa_var *var1, *var2;
	struct ekwa_buffer arg1, arg2, val1, val2;
	uint16_t sum = 0;

	if (!line || line == NULL) {
		printf("\n[E]: ekwa_token_concat args.\n");
		exit(1);
	}

	arg1 = ekwa_decode_buffer(line->arg1);
	arg2 = ekwa_decode_buffer(line->arg2);

	var1 = ekwa_find_var(arg1.data);
	var2 = ekwa_find_var(arg2.data);

	if (!var1 || !var2 || var1 == NULL 
		|| var2 == NULL) {
		printf("\n[E]: Can't find vars in ekwa"
				"_token_concats.\n");
		return;
	}

	val1 = ekwa_decode_buffer(var1->value);
	val2 = ekwa_decode_buffer(var2->value);

	if (val2.length == 0) {
		return;
	}

	sum = val1.length + val2.length;

	if (sum > MAXBUFFER_LEN) {
		printf("\n[E]: Can't concate vars, end sum "
				"of buffer is %d.\n", sum);
		return;
	}

	memcpy(var1->value + 2 + val1.length, val2.data,
			val2.length);
	memcpy(var1->value, &sum, sizeof(uint16_t));
}

void
ekwa_token_set_opt(struct ekwa_instruction *line)
{
	
}
