
#ifndef EKWA_COMPILER
#define EKWA_COMPILER

#include <iostream>
#include <vector>
#include <regex>
#include <string>
#include <map>
#include <thread>
#include <fstream>
#include <unistd.h>
#include <sstream>
#include <inttypes.h>

#define MAXBUFFER_LEN 4000

using namespace std;

const string rgfns = "fn ([a-zA-Z0-9_]+)([\n][{])([a-zA-Z0-9 ><\n\t=+-_\"{}()!,.%/*]+?[\n][}])";
const string rgvar_init = "^([string|int|float|auto]+) ([a-zA-Z0-9_]+) = (.+)";
const string rgcall_m = "^([a-zA-Z0-0_]+[.][a-zA-Z0-9]+)[(](.*[)])$";
const string rgarg = "arg ([string|int|float]+) ([a-zA-Z0-9_]+)";
const string rgvar_noinit_minus = "^([a-zA-Z0-9_]+) [-]= (.+)";
const string rgvar_noinit_plus = "^([a-zA-Z0-9_]+) [+]= (.+)";
const string rg_if = "^if (.+) ([=][=]|[!][=]|[<]|[>]) (.+)$";
const string rgvar_noinit = "^([a-zA-Z0-9_]+) = (.+)";
const string rg_show = "^show[(]([a-zA-Z0-9_]+)[)]$";
const string rg_break = "^break$";
const string rg_exit = "^exit$";
const string rg_loop = "^loop$";

const string rgvar_noinit_div = "^([a-zA-Z0-9_]+) [/]= (.+)";
const string rgvar_noinit_mod = "^([a-zA-Z0-9_]+) [%]= (.+)";
const string rgvar_noinit_mul = "^([a-zA-Z0-9_]+) [*]= (.+)";

enum ekwa_tokens {
	EKWA_VAR	= 0x01, // New var.
	EKWA_BUFF	= 0x02, // Write to buffer.
	EKWA_ARG	= 0x03, // Argument for function.
	EKWA_CALL	= 0x04, // Call system function.
	EKWA_JMP	= 0x05, // Jump to flag.
	EKWA_FSET	= 0x06, // Set flag for jumping.
	EKWA_WRT	= 0x07, // Write from buffer to var.
	EKWA_IFE	= 0x08, // If args are equal.
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
	/* --------------------- */

	EKWA_CAT	= 0x15, // Concatenation.
	EKWA_OPT	= 0x16, // Set/reset vm options.
	EKWA_IFNE	= 0x17, // If args aren't equal.
	EKWA_EXIT	= 0x18, // Stop script.

	EKWA_END	= 0x20
};

enum ekwa_types {
	EKWA_BYTES	= 0x00,
	EKWA_INT	= 0x01,
	EKWA_FLOAT	= 0x02,
	EKWA_CUSTOM	= 0x03
};

struct txtfunction {
	string name;
	string body;
};

struct ekwa_var {
	enum ekwa_types type;
	string name;
};

class _ekwa_instructions
{
	public :
		vector<unsigned char *> exit_s(void);
		vector<unsigned char *> two_args(string, string, unsigned char);
		vector<unsigned char *> one_arg(string, unsigned char);
		vector<unsigned char *> if_body(string, string);
		vector<unsigned char *> loop_end(string, string);
		vector<unsigned char *> new_var(string, enum ekwa_types);
		vector<unsigned char *> set_value(string, string, enum ekwa_types);
		vector<unsigned char *> comparing(string, string, enum ekwa_tokens);
};

class _ekwa_function
{
	private :
		vector<unsigned char *> instructions;
		vector<struct ekwa_var> arguments;
		vector<struct ekwa_var> vars;
		_ekwa_instructions cmd;
		string name;

		void get_if(string, string, string, stringstream &, size_t &);
		void action_var_noinit_div(string, string);
		void action_var_noinit_mul(string, string);
		vector<unsigned char *> action_var_int_mod(string, string);
		void action_var_noinit_mod(string, string);
		vector<unsigned char *> action_var_string(string, string, bool);
		vector<unsigned char *> action_var_float(string, string, unsigned char);
		vector<unsigned char *> action_var_auto(string, string);
		vector<unsigned char *> action_var_int(string, string, unsigned char);
		vector<unsigned char *> add_var(struct ekwa_var);
		void action_var_init(string, string, string);
		vector<struct ekwa_var> find_args(string);
		void change_type(string, enum ekwa_types);
		void action_var_noinit_pl(string, string);
		void action_var_noinit_mn(string, string);
		void get_loop(stringstream &, size_t &);
		void action_funcm_call(string, string);
		void add_cmds(vector<unsigned char *>);
		void action_var_noinit(string, string);
		enum ekwa_types detect_type(string);
		vector<string> get_fnargs(string);
		void code_to_instructions(string);
		struct ekwa_var find_var(string);
		void add_arg(struct ekwa_var);
		void print(unsigned char *);
		void action_show(string);
		bool var_exists(string);
		void set_jump(void);
		void set_exit(void);

	public :
		_ekwa_function(struct txtfunction, _ekwa_instructions);
		void write_file(string);
};

#endif