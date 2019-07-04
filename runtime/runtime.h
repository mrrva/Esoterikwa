
#ifndef EKWA_RUNTIME
#define EKWA_RUNTIME

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>

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

	EKWA_END	= 0x0f  // Last byte token.
};

struct ekwa_arg {
	unsigned char arg[MAXBUFFER_LEN + sizeof(uint16_t)];
	struct ekwa_arg *next;
};

struct ekwa_var {
	unsigned char value[MAXBUFFER_LEN + sizeof(uint16_t)];
	unsigned char name[MAXBUFFER_LEN + sizeof(uint16_t)];
	struct ekwa_var *next;
};

struct ekwa_instruction {
	unsigned char arg1[MAXBUFFER_LEN + sizeof(uint16_t)];
	unsigned char arg2[MAXBUFFER_LEN + sizeof(uint16_t)];
	struct ekwa_instruction *next;
	enum ekwa_tokens token;
};

extern unsigned char _binary_instructions_start[];
extern unsigned char _binary_instructions_end[];
extern const size_t _binary_instructions_size;
struct ekwa_var *ekwa_vars;
struct ekwa_arg *ekwa_args;

bool
ekwa_frombytecode(struct ekwa_instruction **,
				unsigned char *, size_t);

void
ekwa_instruction_clear(struct ekwa_instruction **);

void
ekwa_virtual_machine(struct ekwa_instruction *);

void
ekwa_exception(enum ekwa_tokens, struct ekwa_var *,
			bool);

void
ekwa_token_buffer(struct ekwa_instruction *,
				unsigned char **);

void
ekwa_token_write(struct ekwa_instruction *,
				unsigned char *);

void
ekwa_token_show(struct ekwa_instruction *);

void
ekwa_token_var(struct ekwa_instruction *);

void
ekwa_new_var(struct ekwa_var *);

char *
ekwa_token_name(enum ekwa_tokens);

struct ekwa_var *
ekwa_find_var(unsigned char *);

#endif