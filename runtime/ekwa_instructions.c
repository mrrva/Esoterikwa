
#include "include/runtime.h"

void
ekwa_instruction_add(struct ekwa_instruction **,
					struct ekwa_instruction);


bool 
ekwa_frombytecode(struct ekwa_instruction **list,
				unsigned char *bytes, size_t size)
{
	struct ekwa_instruction line;
	unsigned char *ptr = bytes;
	size_t num = 0;
	uint16_t len;

	if (!bytes || size < 3) {
		return false;
	}

	while (num++, ptr && ptr != NULL) {
		if (*ptr >= EKWA_END || *ptr == 0x00) {
			printf("[E]: Invalid token %x"
				", line: %lu\n", *ptr, num);
			exit(1);
		}

		line.token = *ptr;

		if (!(++ptr)) {
			printf("[E]: Invalid length 1 for %x"
				", line: %lu\n", *ptr, num);
			exit(1);
		}

		memcpy(&len, ptr, sizeof(uint16_t));
		ptr += sizeof(uint16_t);

		if (len > MAXBUFFER_LEN || !ptr) {
			printf("[E]: Invalid arg 1 for %x"
				", line: %lu\n", *ptr, num);
			exit(1);
		}
		else if (len == 0) {
			line.arg1[0] = 0x00;
			line.arg2[0] = 0x00;
			ekwa_instruction_add(list, line);
		}

		memcpy(line.arg1, ptr, len);
		ptr += len;

		if (!ptr) {
			printf("[E]: Invalid length 2 for %x"
				", line: %lu\n", *ptr, num);
			exit(1);
		}

		memcpy(&len, ptr, sizeof(uint16_t));
		ptr += sizeof(uint16_t);

		if (len > MAXBUFFER_LEN || !ptr) {
			printf("[E]: Invalid arg 2 for %x"
				", line: %lu\n", *ptr, num);
			exit(1);
		}
		else if (len == 0) {
			line.arg2[0] = 0x00;
			ekwa_instruction_add(list, line);
		}

		memcpy(line.arg2, ptr, len);
		ekwa_instruction_add(list, line);
		ptr += len;
	}

#ifdef RUNTIME_DEBUG
	printf("[I]: Number of instructions: %lu\n", num);
#endif

	return true;
}

void
ekwa_instruction_add(struct ekwa_instruction **list,
					struct ekwa_instruction new)
{
	size_t len = sizeof(struct ekwa_instruction);
	struct ekwa_instruction *ptr = *list, *one;
	char *name;

	one = (struct ekwa_instruction*)malloc(len);

	if (!one) {
		printf("[E]: Can't allocate memory.\n");
		exit(1);
	} 

	memcpy(one, &new, len);
	one->next = NULL;

	while (ptr->next && ptr->next != NULL) {
		ptr = ptr->next;
	}

#ifdef RUNTIME_DEBUG
	name = ekwa_token_name((char)new.token);
	printf("New token: %s\n", name);
	free(name);
#endif

	ptr->next = one;
}

void
ekwa_instruction_clear(struct ekwa_instruction **list)
{
	struct ekwa_instruction *ptr = *list, *tmp;

	while (ptr && ptr != NULL) {
		tmp = ptr->next;
		free(ptr);
		ptr = tmp;
	}

	*list = NULL;
}

char *
ekwa_token_name(char tok)
{
	char *name;

	if (ptr >= EKWA_END || ptr == 0x00) {
		return NULL;
	}

	switch (toke) {
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

	default:
		name = "EKWA_UNDECLARED";
		break;
	}

	return name;
}
/*
#define RES_FLAG 0xFFFFFFFF

struct ekwa_resources *ekwa_res(unsigned char *, size_t);

const unsigned char flag[] = {
	0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff
};

bool 
ekwa_frombytecode(struct ekwa_instructions **list,
				unsigned char *bytes,
				size_t size)
{
	struct ekwa_resources *res;
	unsigned char *res_p;
	size_t len = 0;

	if (!bytes || size < 3) {
		return false;
	}

	for (size_t i = 0; i < size; i++)
		if (bytes[i] == flag[0] && size - i > 12
			&& memcmp(bytes + i, flag, 10) == 0) {
			res_p = bytes + i + 10;
			len = size - i;
			break;
		}

	if ((res = ekwa_res(res_p, len)) == NULL
		|| !ekwa_replace(list, res, bytes, size - len)) {
		return false;
	}

	return true;
}

struct ekwa_resources *
ekwa_res(unsigned char *p, size_t len)
{
	unsigned char *b_ptr = p, buff[MAXBUFFER_LEN];
	size_t size = sizeof(struct ekwa_resources);
	struct ekwa_resources *list = NULL, *add;
	uint16_t c_flag = 0, c_len = 0;
	size_t l_len = MAXBUFFER_LEN;

	if (!p || len < 1) {
		printf("[W]: ekwa_res.\n");
		return NULL;
	}

	while (b_ptr && b_ptr != NULL) {
		memcpy(c_len, b_ptr, sizeof(uint16_t));

		if (c_len == 0 || !(b_ptr + c_len)) {
			break;
		}

		add = (struct ekwa_resources *)malloc(size);
		l_len = (c_len > l_len) ? l_len : c_len;

		memcpy(add->bytes, b_ptr + 1, l_len);
		add->length = l_len;
		add->flag = c_flag;
		add->next = list;
		list = add;

		b_ptr += c_len + 1;
		c_flag++;
	}

	return list;
}

bool
ekwa_replace(struct ekwa_instructions **list,
			struct ekwa_resources *res,
			unsigned char *p, size_t len)
{
	unsigned char token;
	uint16_t label;

	if (!res || !p || len < 3) {
		printf("[W]: ekwa_replace.\n");
		return false;
	}

	

	return true;
}*/
