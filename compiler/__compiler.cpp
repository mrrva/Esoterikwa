
#include "compiller.hpp"

using namespace std;

vector<struct all_vars> ekwa_vars;
/*unsigned char *_ekwa_(enum ekwa_tokens token,
	struct ekwa_arg arg1, struct ekwa_arg arg2)
{
	size_t b_len = arg1.len + arg2.len, ts;
	unsigned char *buffer, tp;

	if (token >= EKWA_END || b_len > MAXBUFFER_LEN
		|| token == 0x00) {
		cout << "Error: Incorrect args for generation "
			<< "instructions.\n";
		exit(1);
	}

	buffer = new unsigned char[1 + b_len + 50];

	if (!buffer) {
		cout << "Error: Can't allocate memory.\n";
		exit(1);
	}

	tp = static_cast<unsigned char>(token);
	memcpy(buffer, &tp, 1);

	if (arg1.len > 0) {
		memcpy(buffer + 1 + arg1.len, arg1.arg, arg1.len);
		memcpy(buffer + 1, &arg1.len, sizeof(uint16_t));
	}

	ts = 2 + arg1.len;
	memcpy(buffer + ts, &arg2.len, sizeof(uint16_t));

	if (arg2.len > 0) {
		memcpy(buffer + ts + arg2.len, arg2.arg, arg2.len);
	}

	return buffer;
}*/

void print_instructions(unsigned char *list)
{
	uint16_t len = 0, len2 = 0;

	cout << hex << + *list << "\t\t";
	cout << hex << + *(list + 1) << " "
		<< hex <<  + *(list + 2) << "\t";

	memcpy(&len, list + 1, sizeof(uint16_t));

	for (size_t i = 0; i < len; i++) {
		cout << hex << + list[i + 3] << " ";
	}

	memcpy(&len2, list + 3 + len, sizeof(uint16_t));
	cout << "\t\t";

	cout << hex << + *(list + 3 + len) << " " << hex
		<< + *(list + 4 + len) << "\t";

	for (size_t i = 0; i < len2; i++) {
		cout << hex << + list[i + 5 + len] << " ";
	}

	cout << endl;
}

unsigned char *_ekwa_new_var(string name, enum ekwa_types type)
{
	unsigned char *buffer, token = EKWA_VAR, tp;
	uint16_t len = name.length(), tmp = 1;
	const char *c_name = name.c_str();
	struct all_vars var;

	if (len > MAXBUFFER_LEN || len == 0) {
		cout << "Error: _ekwa_var for " << name << endl;
		exit(1);
	}

	buffer = new unsigned char[20 + len];
	tp = static_cast<unsigned char>(type);

	if (!buffer) {
		cout << "Error: Can't allocate memory.\n";
		exit(1);
	}

	memcpy(buffer, &token, 1);
	memcpy(buffer + 1, &len, 2);
	memcpy(buffer + 3, c_name, len);
	memcpy(buffer + 3 + len, &tmp, 2);
	memcpy(buffer + 6 + len, &tp, 1);

	var.type = type;
	var.name = name;

	ekwa_vars.push_back(var);

	return buffer;
}

unsigned char *_ekwa_call_func(string name)
{
	unsigned char *buffer, token = EKWA_CALL;
	uint16_t len = name.length(), nl = 0;
	const char *fname = name.c_str();

	if (len == 0 || len > MAXBUFFER_LEN) {
		cout << "Error: Name of function is more then "
			<< MAXBUFFER_LEN << endl;
		exit(1);
	}

	buffer = new unsigned char[len + 50];

	if (!buffer) {
		cout << "Error: Can't allocate memory.\n";
		exit(1);
	}

	memcpy(buffer, &token, 1);
	memcpy(buffer + 1, &len, 2);
	memcpy(buffer + 3, fname, len);
	memcpy(buffer + 4 + len, &nl, 2);

	return buffer;
}

vector<unsigned char *> _ekwa_from_buffer(string name)
{
	vector<unsigned char *> commands;
	unsigned char *buffer, token = EKWA_WRT;
	uint16_t len = name.length(), nl = 0;
	const char *arg = name.c_str();
	bool exists = false;

	if (len == 0 || len > MAXBUFFER_LEN) {
		cout << "Error: Incorrect name of var.\n";
		exit(1);
	}

	for (auto &p : ekwa_vars) {
		if (p.name == name) {
			exists = true;
			break;
		}
	}

	if (!exists) {
		buffer = _ekwa_new_var(name, EKWA_BYTES);
		print_instructions(buffer);
		commands.push_back(buffer);
	}

	buffer = new unsigned char[len + 50];

	if (!buffer) {
		cout << "Error: Can't allocate memory.\n";
		exit(1);
	}

	memcpy(buffer, &token, 1);
	memcpy(buffer + 1, &len, 2);
	memcpy(buffer + 3, arg, len);
	memcpy(buffer + 4 + len, &nl, 2);

	print_instructions(buffer);
	commands.push_back(buffer);
	return commands;
}


unsigned char *_ekwa_var_val(string name, string value,
	enum ekwa_types type)
{
	unsigned char *buffer, token = EKWA_VAL, tp;
	uint16_t n_len = name.length(), v_len;
	const char *c_name = name.c_str(), *bval;
	float fval = 0;
	int ival = 0;

	if (n_len == 0 || n_len > MAXBUFFER_LEN) {
		cout << "Skip: _ekwa_var_val - " << name << ": "
			<< value << endl;
		return NULL;
	}

	buffer = new unsigned char[20 + n_len + value.length()];
	tp = static_cast<unsigned char>(type);

	if (!buffer) {
		cout << "Error: Can't allocate memory.\n";
		exit(1);
	}

	memcpy(buffer, &token, 1);

	memcpy(buffer + 1, &n_len, 2);
	memcpy(buffer + 3, c_name, n_len);

	switch (type) {
		case EKWA_INT:
			ival = stoi(value);
			v_len = sizeof(ival);

			memcpy(buffer + 5 + n_len, &ival, v_len);
			memcpy(buffer + 3 + n_len, &v_len, 2);
			break;

		case EKWA_FLOAT:
			fval = stof(value);
			v_len = sizeof(fval);

			memcpy(buffer + 5 + n_len, &fval, v_len);
			memcpy(buffer + 3 + n_len, &v_len, 2);
			break;

		default:
			v_len = value.length();
			bval = value.c_str();

			if (v_len > MAXBUFFER_LEN) {
				cout << "Error: " << name << " has length"
					<< " more then " << MAXBUFFER_LEN << endl;
				exit(1);
			}

			memcpy(buffer + 5 + n_len, bval, v_len);
			memcpy(buffer + 3 + n_len, &v_len, 2);
			break;
	}

	return buffer;
}

vector<unsigned char *> _ekwa_set_args(string args)
{
	regex rgx_args("([a-zA-Z0-9_]+)");
	vector<unsigned char *> lines;
	unsigned char *buffer, token = EKWA_ARG;
	const char *name;
	uint16_t len = 0, tp = 0;
	bool exists = false;
	smatch match;

	if (args.length() > MAXBUFFER_LEN) {
		cout << "Error: Incorrect length of args.\n";
		exit(1);
	}

	if (!regex_search(args, match, rgx_args)) {
		return lines;
	}

	while (regex_search(args, match, rgx_args)) {
		for (auto &p : ekwa_vars) {
			if (p.name == match.str(0)) {
				exists = true;
				break;
			}
		}

		if (!exists) {
			cout << "Error: Var " << match.str(0)
				<< " doesn't" << "exists.\n";
			exit(1);
		}

		len = match.str(0).length();
		buffer = new unsigned char[len + 50];
		name = match.str(0).c_str();

		if (!buffer) {
			cout << "Error: Can't allocate memory.\n";
			exit(1);
		}

		memcpy(buffer, &token, 1);
		memcpy(buffer + 1, &len, 2);
		memcpy(buffer + 3, name, len);
		memcpy(buffer + 4 + len, &tp, 2);

		print_instructions(buffer);
		lines.push_back(buffer);
		args = match.suffix();
		exists = false;
	}

	return lines;

//([a-zA-Z0-9_]+|"[a-zA-Z0-9_]+"|([0-9]*[.][0-9]*))
}

vector<struct ekwa_args> detect_fn_args(string s)
{
	regex rgx("[int|float|string|auto]+ [a-zA-Z0-9_]+");
	vector<struct ekwa_args> args;
	struct ekwa_args one;
	stringstream ss;
	smatch match;
	string tmp;

	if (!regex_search(s, match, rgx)
		|| match.size() == 0) {
		return args;
	}

	while (regex_search(s, match, rgx)) {
		ss = stringstream(match.str(0));
		ss >> tmp;

		if (tmp == "int") {
			one.type = EKWA_INT;
		}

		else if (tmp == "float") {
			one.type = EKWA_FLOAT;
		}

		else if (tmp == "string") {
			one.type = EKWA_BYTES;
		}

		else if (tmp == "auto") {
			one.type = EKWA_CUSTOM;
		}

		else {
			cout << "Error: Incorrect type of var"
				<< " in " << s << endl;
			exit(1);
		}

		ss >> one.name;
		s = match.suffix();

		args.push_back(one);
	}

	return args;
}

struct fn_title function_title(string s)
{
	regex rgx("fn ([a-zA-Z0-9_]+)(.+):");
	struct fn_title func;
	smatch match;
	
	if (!regex_search(s, match, rgx)) {
		cout << "Error: Incorrect line " << s << endl;
		exit(1);
	}

	if (match.size() == 0 || match.str(1).length() < 2) {
		cout << "Error: Incorrect line " << s << endl;
		exit(1);
	}

	func.args = detect_fn_args(match.str(2));
	func.name = match.str(1);

	return func;
}

bool var_exists_val(vector<unsigned char *> &list,
	string name, string value, enum ekwa_types type)
{
	unsigned char *buffer;

	for (auto &p : ekwa_vars)
		if (p.name == name) {
			if (p.type != type) {
				cout << "Error: Can't set value for "
					<< name << ", incorrect type.\n";
				exit(1);
			}

			buffer = _ekwa_var_val(name, value, type);
			list.push_back(buffer);
			print_instructions(buffer);
			return true;
		}

	return false;
}

vector<unsigned char *> set_int_var(string name,
	string value)
{
	vector<unsigned char *> list;
	unsigned char *buffer;

	if (var_exists_val(list, name, value, EKWA_INT)) {
		return list;
	}

	buffer = _ekwa_new_var(name, EKWA_INT);
	list.push_back(buffer);
	print_instructions(buffer);
	
	buffer = _ekwa_var_val(name, value, EKWA_INT);
	list.push_back(buffer);
	print_instructions(buffer);

	return list;
}

vector<unsigned char *> set_float_var(string name,
	string value)
{
	vector<unsigned char *> list;
	unsigned char *buffer;

	if (var_exists_val(list, name, value, EKWA_FLOAT)) {
		return list;
	}

	buffer = _ekwa_new_var(name, EKWA_FLOAT);
	list.push_back(buffer);
	print_instructions(buffer);

	buffer = _ekwa_var_val(name, value, EKWA_FLOAT);
	list.push_back(buffer);
	print_instructions(buffer);

	return list;
}

vector<unsigned char *> set_byte_var(string name,
	string value)
{
	vector<unsigned char *> list;
	unsigned char *buffer;

	if (var_exists_val(list, name, value, EKWA_BYTES)) {
		return list;
	}

	buffer = _ekwa_new_var(name, EKWA_BYTES);
	list.push_back(buffer);
	print_instructions(buffer);

	buffer = _ekwa_var_val(name, value, EKWA_BYTES);
	list.push_back(buffer);
	print_instructions(buffer);

	return list;
}

vector<unsigned char *> set_call_noargs_var(string name,
	string fname)
{
	vector<unsigned char *> list, tmp;
	unsigned char *buffer;

	buffer = _ekwa_call_func(fname);
	list.push_back(buffer);
	print_instructions(buffer);

	tmp = _ekwa_from_buffer(name);
	list.insert(list.end(), tmp.begin(), tmp.end());
//print_instructions(buffer);
	return list;
}

vector<unsigned char *> set_call_noargs(string fname)
{
	vector<unsigned char *> list;
	unsigned char *buffer;

	buffer = _ekwa_call_func(fname);
	list.push_back(buffer);
	print_instructions(buffer);

	return list;
}

vector<unsigned char *> set_call_args(string fname,
	string args)
{
	vector<unsigned char *> list, tmp;
	unsigned char *buffer;

	tmp = _ekwa_set_args(args);
	list.insert(list.end(), tmp.begin(), tmp.end());

	buffer = _ekwa_call_func(fname);
	print_instructions(buffer);
	list.push_back(buffer);

	return list;
}

vector<unsigned char *> set_call_args_var(string name,
	string fname, string args)
{
	vector<unsigned char *> list = set_call_args(fname, args), tmp;
	unsigned char *buffer;

	tmp = _ekwa_from_buffer(name);
	list.insert(list.end(), tmp.begin(), tmp.end());

	return list;
}

vector<unsigned char *> operats_var(string name, string ops)
{
	regex rgx_vars("([a-zA-Z0-9_]+|[+-])");
	vector<unsigned char *> list;
}

vector<unsigned char *> command_line(string s,
	struct fn_title fntitle)
{
	regex rgx_callvar_args("\t([a-zA-Z0-9_]+) = ([a-zA-Z0-9_]+.[a-zA-Z0-9_]+)[(](.+)[)]+$");
	regex rgx_callvar_noargs("\t([a-zA-Z0-9_]+) = ([a-zA-Z0-9_]+.[a-zA-Z0-9_]+)[(][)]+$");
	regex rgx_call_args("\t([a-zA-Z0-9_]+.[a-zA-Z0-9_]+)[(](.+)[)]+$");
	regex rgx_call_noargs("\t([a-zA-Z0-9_]+.[a-zA-Z0-9_]+)[(][)]+$");
	regex rgx_float("\t([a-zA-Z0-9_]+) = ([0-9]+).([0-9]+)$");
	regex rgx_byte("\t([a-zA-Z0-9_]+) = \"(.*?)\"$");
	regex rgx_int("\t([a-zA-Z0-9_]+) = ([0-9]+)$");


	regex rgx_operats("\t([a-zA-Z0-9_]+) = ([a-zA-Z0-9_ +-]+)$");

	vector<unsigned char *> line;
	vector<unsigned char *> step;
	smatch match;

	if (s.length() < 3) {
		cout << "Passed line: " << s << endl;
		return line;
	}

	if (regex_search(s, match, rgx_int)) {
		step = set_int_var(match.str(1), match.str(2));
		cout << "--------------------------------------------------------------------\n";
		//line.insert(line.end(), step.begin(), step.end());
	}

	else if (regex_search(s, match, rgx_float)) {
		step = set_float_var(match.str(1), match.str(2) +
				"." + match.str(3));
		cout << "--------------------------------------------------------------------\n";
		//line.insert(line.end(), step.begin(), step.end());
	}

	else if (regex_search(s, match, rgx_byte)) {
		step = set_byte_var(match.str(1), match.str(2));
		cout << "--------------------------------------------------------------------\n";
		//line.insert(line.end(), step.begin(), step.end());
	}

	else if (regex_search(s, match, rgx_callvar_noargs)) {
		step = set_call_noargs_var(match.str(1), match.str(2));
		cout << "--------------------------------------------------------------------\n";
	}

	else if (regex_search(s, match, rgx_call_noargs)) {
		step = set_call_noargs(match.str(1));
		cout << "--------------------------------------------------------------------\n";
	}

	else if (regex_search(s, match, rgx_call_args)) {
		step = set_call_args(match.str(1), match.str(2));
		cout << "--------------------------------------------------------------------\n";
	}

	else if (regex_search(s, match, rgx_callvar_args)) {
		step = set_call_args_var(match.str(1), match.str(2),
				match.str(3));
		cout << "--------------------------------------------------------------------\n";
	}

	else if (regex_search(s, match, rgx_operats)) {
		step = operats_var(match.str(1), match.str(2));
		cout << "--------------------------------------------------------------------\n";
	}

	return line;
}

int main(int argc, char *args[])
{
	/*vector<string> source_file;

	if (argc < 2) {
		cout << "Error: Haven't input file.\n";
		return 1;
	}

	for (size_t i = 1; i < argc; i++) {
		source_files.push_back(string(args[i]));
	}

	for (auto &file : source_files) {
		thread(read_source, file).detach();
	}*/
	vector<unsigned char *> cmds;
	struct fn_content fcontent;
	string line;
	fstream text;

	if (argc < 2) {
		cout << "Error: Haven't input file.\n";
		return 1;
	}

	text = fstream(string(args[1]));

	if (!text.is_open()) {
		cout << "Error: Incorrect path to "
			<< "source file." << endl;
		exit(1);
	}

	while (getline(text, line, '\n')) {
		if (line.length() < 2) {
			continue;
		}

		if (line.substr(0, 2) == "fn") {
			fcontent.title = function_title(line);
		}

		if (fcontent.title.name.length() != 0
			&& line[0] == '\t') {
			cmds = command_line(line,  fcontent.title);
			//fcontent.body.push_back(line.substr(1));
		}
	}
}