
#ifndef EKWA_RUNTIME
#define EKWA_RUNTIME

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define RUNTIME_DEBUG true
#define MAXBUFFER_LEN 4000
#define INFO_BLOCK "0.1 - Esoterikwa Virtual Machine"
					" - June 2019.\n(c) mrrva.\n"

extern unsigned char _binary_instructions_start[];
extern unsigned char _binary_instructions_end[];
extern const size_t _binary_instructions_size;

enum ekwa_tokens {
	EKWA_VAR	= 0x01, // New var.
	EKWA_BUFF	= 0x02, // Write to buffer.
	EKWA_ARG	= 0x03, // Argument for function.
	EKWA_CALL	= 0x04, // Call system function.
	EKWA_JMP	= 0x05, // Jump to label.
	EKWA_FSET	= 0x06, // Set flag for jumping.
	EKWA_WRT	= 0x07, // Write from buffer to var.
	EKWA_CMP	= 0x07, // If args are equal.
	EKWA_IFS	= 0x08, // If arg is smoller.
	EKWA_IFB	= 0x0a, // If arg is bigger.
	EKWA_INFO	= 0x0b, // Info about VM.

	EKWA_END	= 0x0c // Last byte token.
};

struct ekwa_args {

};

struct ekwa_vars {

};

struct ekwa_instruction {
	unsigned char arg1[MAXBUFFER_LEN];
	unsigned char arg2[MAXBUFFER_LEN];
	struct ekwa_instruction *next;
	enum ekwa_tokens token;
};

bool
ekwa_frombytecode(struct ekwa_instruction **,
				unsigned char *, size_t);

void
ekwa_instruction_clear(struct ekwa_instruction *);

#endif