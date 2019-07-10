
#ifndef EKWA_RUNTIME
#define EKWA_RUNTIME

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <dlfcn.h>

#define RUNTIME_DEBUG true
#define MAXBUFFER_LEN 4000
#define INFO_BLOCK "0.1 - Esoterikwa Virtual Machine" \
					" - June 2019.\n(c) mrrva.\n\n"

enum ekwa_tokens {
	EKWA_VAR	= 0x01, // New var.
	EKWA_BUFF	= 0x02, // Write to buffer.
	EKWA_ARG	= 0x03, // Argument for function.
	EKWA_CALL	= 0x04, // Call system function.
	EKWA_JMP	= 0x05, // Jump to flag.
	EKWA_FSET	= 0x06, // Set flag for jumping.
	EKWA_WRT	= 0x07, // Write from buffer to var.
	EKWA_CMP	= 0x08, // If args are equal.
	EKWA_IFS	= 0x0a, // If arg is smoller.
	EKWA_IFB	= 0x0b, // If arg is bigger.
	EKWA_INFO	= 0x0c, // Info about VM.
	EKWA_SHOW	= 0x0d, // Print var value as a string.
	EKWA_RMV	= 0x0e, // Remove var.
	EKWA_VAL	= 0x0f, // Set value of var.

	/* Arithmetic operations */
	EKWA_ADD	= 0x10,
	EKWA_SUB	= 0x11,
	EKWA_DIV	= 0x12,
	EKWA_MOD	= 0x13,
	EKWA_MUL	= 0x14,

	EKWA_END	= 0x15
};

enum ekwa_types {
	EKWA_BYTES	= 0x00,
	EKWA_INT	= 0x01,
	EKWA_FLOAT	= 0x02,
	EKWA_CUSTOM	= 0x03
};

struct ekwa_buffer {
	unsigned char data[MAXBUFFER_LEN + 1];
	uint16_t length;
};

struct ekwa_var {
	unsigned char value[MAXBUFFER_LEN + sizeof(uint16_t)];
	char name[MAXBUFFER_LEN + 1];
	enum ekwa_types type;
	struct ekwa_var *next;
};

struct ekwa_instruction {
	unsigned char arg1[MAXBUFFER_LEN + sizeof(uint16_t)];
	unsigned char arg2[MAXBUFFER_LEN + sizeof(uint16_t)];
	struct ekwa_instruction *next;
	enum ekwa_tokens token;
};

struct ekwa_flag {
	char name[MAXBUFFER_LEN + 1];
	struct ekwa_instruction *point;
	struct ekwa_flag *next;
};

struct ekwa_option {
	char name[MAXBUFFER_LEN + 1];
	unsigned char value[MAXBUFFER_LEN];
	struct ekwa_option *next;
};

struct ekwa_arg {
	unsigned char value[MAXBUFFER_LEN + sizeof(uint16_t)];
	enum ekwa_types type;
	struct ekwa_arg *next;
};

extern unsigned char _binary_instructions_start[];
extern unsigned char _binary_instructions_end[];
extern const size_t _binary_instructions_size;
struct ekwa_option *ekwa_opts;
struct ekwa_flag *ekwa_flags;
struct ekwa_var *ekwa_vars;
struct ekwa_arg *ekwa_args;

bool
ekwa_frombytecode(struct ekwa_instruction **,
				unsigned char *, size_t);

void
ekwa_instruction_clear(struct ekwa_instruction **);

struct ekwa_flag *
ekwa_get_flag(char *);

void
ekwa_virtual_machine(struct ekwa_instruction *);

void
ekwa_token_jump(struct ekwa_instruction **);

void
ekwa_set_flags(struct ekwa_instruction *);

void
ekwa_token_buffer(struct ekwa_instruction *,
				struct ekwa_var *);

void
ekwa_token_write(struct ekwa_instruction *,
				struct ekwa_var *);

void
ekwa_token_comparing(struct ekwa_instruction **);

void
ekwa_token_show(struct ekwa_instruction *);

void
ekwa_token_var(struct ekwa_instruction *);

void
ekwa_new_var(struct ekwa_var *);

char *
ekwa_token_name(enum ekwa_tokens);

struct ekwa_var *
ekwa_find_var(char *);

void
ekwa_token_remove_var(struct ekwa_instruction *);

void
ekwa_token_add_arg(struct ekwa_instruction *);

void
ekwa_token_call(struct ekwa_instruction *,
				struct ekwa_var *);

void
ekwa_token_rbuffer(struct ekwa_instruction *,
					unsigned char *);

struct ekwa_buffer
ekwa_decode_buffer(unsigned char *);

enum ekwa_types
ekwa_detect_type(unsigned char *);

void
ekwa_token_set_value(struct ekwa_instruction *);

int
ekwa_buffer_to_int(unsigned char *);

float
ekwa_buffer_to_float(unsigned char *);

void
ekwa_token_ifsmaller(struct ekwa_instruction **);

void
ekwa_token_ifbigger(struct ekwa_instruction **);

void
ekwa_token_math(struct ekwa_instruction *,
				struct ekwa_var *,
				enum ekwa_tokens);

#endif