
#include "compiler.hpp"

_ekwa_function::_ekwa_function(struct txtfunction data,
	_ekwa_instructions do_cmd)
{
	if (data.name.length() == 0 || data.body.length() == 0) {
		cout << "Error 1.\n";
		exit(1);
	}

	this->arguments = this->find_args(data.body);
	this->name = data.name;
	this->cmd = do_cmd;

	this->code_to_instructions(data.body);
}

vector<struct ekwa_var> _ekwa_function::find_args(string s)
{
	enum ekwa_types type = EKWA_BYTES;
	vector<struct ekwa_var> args;
	regex rgx_arg(rgarg);
	smatch match;
	string fullname;

	while (regex_search(s, match, rgx_arg)) {
		fullname = "_fn" + this->name + "_" + match.str(2);
		type = this->detect_type(match.str(1));

		this->add_arg({type, fullname});
		this->add_var({type, fullname});
		s = match.suffix();
	}

	return args;
}

void _ekwa_function::code_to_instructions(string code)
{
	stringstream ss(code);
	size_t flag_n = 0;
	smatch match;
	string line;

	regex rg_4(rgvar_noinit_minus);
	regex rg_3(rgvar_noinit_plus);
	regex rg_2(rgvar_noinit);
	regex rg_1(rgvar_init);
	regex rg_5(rgcall_m);
	regex rg_6(rg_conds);

	while (getline(ss, line, '\n')) {
		if (regex_search(line, match, rg_1)) {
			this->action_var_init(match.str(1),
					match.str(2), match.str(3));
			continue;
		}

		if (regex_search(line, match, rg_3)) {
			this->action_var_noinit_pl(match.str(1),
					match.str(2));
			continue;
		}

		if (regex_search(line, match, rg_4)) {
			this->action_var_noinit_mn(match.str(1),
					match.str(2));
			continue;
		}

		if (regex_search(line, match, rg_5)) {
			this->action_funcm_call(match.str(1),
					match.str(2));
			continue;
		}

		if (regex_search(line, match, rg_6)) {
			this->get_if(match.str(1), match.str(2),
				match.str(3), ss, flag_n);
			continue;
		}
	}
}

void _ekwa_function::get_if(string var, string cond, string val,
	stringstream &ss, size_t &fnum)
{
	vector<unsigned char *> list, tmp;
	string line, code(""), fn1, fn2;
	enum ekwa_tokens tp = EKWA_CMP;
	struct ekwa_var tvar;

	if (!this->var_exists(var)) {
		cout << "Error: 31.\n";
		exit(1);
	}

	while (getline(ss, line, '\n')) {
		if (line[0] == '\t') {
			code += line.substr(1) + "\n";
			continue;
		}

		if (line[0] == '{') {
			continue;
		}

		break;
	}

	tvar = this->find_var(var);

	if (cond == ">") {
		tp = EKWA_IFB;
	}

	if (cond == "<") {
		tp = EKWA_IFS;
	}

	//tmp = this->cmd.comparing(var, var_ 2, tp);
	//list.insert(list.end(), tmp.begin(), tmp.end());

	fn1 = "flag_" + to_string(fnum++);
	fn2 = "flag_" + to_string(fnum++);

	tmp = this->cmd.if_body(fn1, fn2);
	list.insert(list.end(), tmp.begin(), tmp.end());

	this->add_cmds(list);
	exit(1);
}

void _ekwa_function::action_funcm_call(string name,
	string arg_str)
{
	vector<string> args = this->get_fnargs(arg_str);
	vector<unsigned char *> list, tmp;

	for (auto &p : args) {
		tmp = this->cmd.to_arg(p);
		list.insert(list.end(), tmp.begin(), tmp.end());
	}

	tmp = this->cmd.call_function(name);
	list.insert(list.end(), tmp.begin(), tmp.end());

	this->add_cmds(list);
}

void _ekwa_function::action_var_noinit_mn(string name,
	string action)
{
	struct ekwa_var tvar = this->find_var(name);
	vector<unsigned char *> buff;

	if (!this->var_exists(name)) {
		cout << "Error: 25.\n";
		exit(1);
	}

	switch (tvar.type) {
	case EKWA_INT:
		buff = this->action_var_int_mn(name, action);
		break;

	case EKWA_FLOAT:
		buff = this->action_var_float_mn(name, action);
		break;

	default:
		cout << "Error: 26.\n";
		exit(1);
	}

	this->add_cmds(buff);
}

vector<unsigned char *> _ekwa_function::action_var_int_mn(string name,
	string action)
{
	vector<unsigned char *> list, tmp;
	regex var("^([a-zA-Z0-9_]+)$");
	regex val("^([0-9]+)$");
	struct ekwa_var tvar;
	smatch match;

	string tmp_name = "_action_var_int_mn";

	if (regex_search(action, match, val)) {
		// Create new var
		tmp = this->cmd.new_var(tmp_name, EKWA_INT);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Add value to var
		tmp = this->cmd.set_value(tmp_name, match.str(1),
			EKWA_INT);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Call `sub` token
		tmp = this->cmd.minus(name, tmp_name);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Write buffer's data to the var
		tmp = this->cmd.from_buffer_to(name);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Remove tmp var
		tmp = this->cmd.remove_var(tmp_name);
		list.insert(list.end(), tmp.begin(), tmp.end());
	}
	else if (regex_search(action, match, var)) {
		tvar = this->find_var(match.str(1));

		if (tvar.type != EKWA_INT) {
			cout << "Error: 29.\n";
			exit(1);
		}
		// Call `sub` token
		tmp = this->cmd.minus(name, match.str(1));
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Write buffer's data to the var
		tmp = this->cmd.from_buffer_to(name);
		list.insert(list.end(), tmp.begin(), tmp.end());
	}

	return list;
}

vector<unsigned char *> _ekwa_function::action_var_float_mn(string name,
	string action)
{
	vector<unsigned char *> list, tmp;
	regex val("^([0-9]+[.][0-9]+)$");
	regex var("^([a-zA-Z0-9_]+)$");
	struct ekwa_var tvar;
	smatch match;

	string tmp_name = "_action_var_float_mn";

	if (regex_search(action, match, val)) {
		// Create new var
		tmp = this->cmd.new_var(tmp_name, EKWA_FLOAT);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Add value to var
		tmp = this->cmd.set_value(tmp_name, match.str(1),
			EKWA_FLOAT);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Call `sub` token
		tmp = this->cmd.minus(name, tmp_name);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Write buffer's data to the var
		tmp = this->cmd.from_buffer_to(name);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Remove tmp var
		tmp = this->cmd.remove_var(tmp_name);
		list.insert(list.end(), tmp.begin(), tmp.end());
	}
	else if (regex_search(action, match, var)) {
		tvar = this->find_var(match.str(1));

		if (tvar.type != EKWA_FLOAT) {
			cout << "Error: 27.\n";
			exit(1);
		}
		// Call `sub` token
		tmp = this->cmd.minus(name, match.str(1));
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Write buffer's data to the var
		tmp = this->cmd.from_buffer_to(name);
		list.insert(list.end(), tmp.begin(), tmp.end());
	}

	return list;
}

void _ekwa_function::action_var_noinit_pl(string name,
	string action)
{
	struct ekwa_var tvar = this->find_var(name);
	vector<unsigned char *> buff;

	if (!this->var_exists(name)) {
		cout << "Error: 20.\n";
		exit(1);
	}

	switch (tvar.type) {
	case EKWA_BYTES:
		buff = this->action_var_string_pl(name, action);
		break;

	case EKWA_INT:
		buff = this->action_var_int_pl(name, action);
		break;

	case EKWA_FLOAT:
		buff = this->action_var_float_pl(name, action);
		break;

	default:
		cout << "Error: 24.\n";
		exit(1);
	}

	this->add_cmds(buff);
}

vector<unsigned char *> _ekwa_function::action_var_float_pl(string name,
	string action)
{
	vector<unsigned char *> list, tmp;
	regex val("^([0-9]+[.][0-9]+)$");
	regex var("^([a-zA-Z0-9_]+)$");
	struct ekwa_var tvar;
	smatch match;

	string tmp_name = "_action_var_float_pl";

	if (regex_search(action, match, val)) {
		// Create new var
		tmp = this->cmd.new_var(tmp_name, EKWA_FLOAT);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Add value to var
		tmp = this->cmd.set_value(tmp_name, match.str(1),
			EKWA_FLOAT);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Call `add` token
		tmp = this->cmd.plus(name, tmp_name);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Write buffer's data to the var
		tmp = this->cmd.from_buffer_to(name);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Remove tmp var
		tmp = this->cmd.remove_var(tmp_name);
		list.insert(list.end(), tmp.begin(), tmp.end());
	}
	else if (regex_search(action, match, var)) {
		tvar = this->find_var(match.str(1));

		if (tvar.type != EKWA_FLOAT) {
			cout << "Error: 23.\n";
			exit(1);
		}
		// Call `add` token
		tmp = this->cmd.plus(name, match.str(1));
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Write buffer's data to the var
		tmp = this->cmd.from_buffer_to(name);
		list.insert(list.end(), tmp.begin(), tmp.end());
	}

	return list;
}

vector<unsigned char *> _ekwa_function::action_var_int_pl(string name,
	string action)
{
	vector<unsigned char *> list, tmp;
	regex var("^([a-zA-Z0-9_]+)$");
	regex val("^([0-9]+)$");
	struct ekwa_var tvar;
	smatch match;

	string tmp_name = "_action_var_int_pl";

	if (regex_search(action, match, val)) {
		// Create new var
		tmp = this->cmd.new_var(tmp_name, EKWA_INT);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Add value to var
		tmp = this->cmd.set_value(tmp_name, match.str(1),
			EKWA_INT);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Call `add` token
		tmp = this->cmd.plus(name, tmp_name);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Write buffer's data to the var
		tmp = this->cmd.from_buffer_to(name);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Remove tmp var
		tmp = this->cmd.remove_var(tmp_name);
		list.insert(list.end(), tmp.begin(), tmp.end());
	}
	else if (regex_search(action, match, var)) {
		tvar = this->find_var(match.str(1));

		if (tvar.type != EKWA_INT) {
			cout << "Error: 22.\n";
			exit(1);
		}
		// Call `add` token
		tmp = this->cmd.plus(name, match.str(1));
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Write buffer's data to the var
		tmp = this->cmd.from_buffer_to(name);
		list.insert(list.end(), tmp.begin(), tmp.end());
	}

	return list;
}

vector<unsigned char *> _ekwa_function::action_var_string_pl(string name,
	string action)
{
	vector<unsigned char *> list, tmp;
	regex var("^([a-zA-Z0-9_]+)$");
	regex val("\"(.+?)\"$");
	struct ekwa_var tvar;
	smatch match;

	string tmp_name = "_action_var_string_pl";

	if (regex_search(action, match, val)) {
		// Create new var
		tmp = this->cmd.new_var(tmp_name, EKWA_BYTES);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Add value to var
		tmp = this->cmd.set_value(tmp_name, match.str(1),
			EKWA_BYTES);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Call concat token
		tmp = this->cmd.concat(name, tmp_name);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Remove tmp var
		tmp = this->cmd.remove_var(tmp_name);
		list.insert(list.end(), tmp.begin(), tmp.end());
	}
	else if (regex_search(action, match, var)) {
		tvar = this->find_var(match.str(1));

		if (tvar.type != EKWA_BYTES) {
			cout << "Error: 21.\n";
			exit(1);
		}
		// Write var to the buffer
		tmp = this->cmd.concat(name, match.str(1));
		list.insert(list.end(), tmp.begin(), tmp.end());
	}

	return list;
}

void _ekwa_function::action_var_noinit(string name, string action)
{
	struct ekwa_var tvar = this->find_var(name);
	vector<unsigned char *> buff;

	if (!this->var_exists(name)) {
		cout << "Error: 19.\n";
		exit(1);
	}

	switch (tvar.type) {
	case EKWA_BYTES:
		buff = this->action_var_string(name, action);
		break;

	case EKWA_INT:
		buff = this->action_var_int(name, action);
		break;

	case EKWA_FLOAT:
		buff = this->action_var_float(name, action);
		break;

	case EKWA_CUSTOM:
		buff = this->action_var_auto(name, action);
		break;
	}

	this->add_cmds(buff);
}

void _ekwa_function::action_var_init(string type, string name,
	string action)
{
	enum ekwa_types v_type = this->detect_type(type);
	vector<unsigned char *> buff;

	if (this->var_exists(name)) {
		cout << "Error: 13.\n";
		exit(1);
	}

	buff = this->add_var({v_type, name});
	this->add_cmds(buff);

	switch (v_type) {
	case EKWA_BYTES:
		buff = this->action_var_string(name, action);
		break;

	case EKWA_INT:
		buff = this->action_var_int(name, action);
		break;

	case EKWA_FLOAT:
		buff = this->action_var_float(name, action);
		break;

	case EKWA_CUSTOM:
		buff = this->action_var_auto(name, action);
		break;
	}

	this->add_cmds(buff);
}

void _ekwa_function::change_type(string name, enum ekwa_types type)
{
	for (auto &p : this->vars) {
		if (p.name == name) {
			p.type = type;
			return;
		}
	}
}

vector<unsigned char *> _ekwa_function::action_var_auto(string name,
	string action)
{
	vector<unsigned char *> list, tmp;
	regex var("^([a-zA-Z0-9_]+)$");
	regex fun(rgcall_m);
	vector<string> args;
	struct ekwa_var tvar;
	smatch match;

	if (regex_search(action, match, var)) {
		if (!this->var_exists(match.str(1))) {
			cout << "Error: 15.\n";
			exit(1);
		}
		// Find var
		tvar = this->find_var(match.str(1));
		// Write var to the buffer
		tmp = this->cmd.to_buffer(match.str(1));
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Write buffer's data to var
		tmp = this->cmd.from_buffer_to(name);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Change type in the list
		this->change_type(name, tvar.type);
	}
	else if (regex_search(action, match, fun)) {
		// Get vars which are arguments for function
		args = this->get_fnargs(match.str(2));
		// Checking vars
		for (auto &p : args) {
			// Add current var as argument
			tmp = this->cmd.to_arg(p);
			list.insert(list.end(), tmp.begin(), tmp.end());
		}
		// Call function
		tmp = this->cmd.call_function(match.str(1));
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Write buffer's data to var
		tmp = this->cmd.from_buffer_to(name);
		list.insert(list.end(), tmp.begin(), tmp.end());
	}

	return list;
}

vector<string> _ekwa_function::get_fnargs(string line)
{
	regex var("(([a-zA-Z0-9_]+)[,]|([a-zA-Z0-9_]+)[)])");
	vector<string> list;
	smatch match;
	string a;

	if (line.length() < 2) {
		return list;
	}

	while (regex_search(line, match, var)) {
		a = match.str(1).substr(0, match.str(1).length() - 1);
		
		if (!this->var_exists(a)) {
			cout << "Error: 16 - " << a << ".\n";
			exit(1);
		}

		list.push_back(a);
		line = match.suffix();
	}

	return list;
}

vector<unsigned char *> _ekwa_function::action_var_float(string name,
	string action)
{
	vector<unsigned char *> list, tmp;
	regex val("^([0-9]+[.][0-9]+)$");
	regex var("^([a-zA-Z0-9_]+)$");
	struct ekwa_var tvar;
	smatch match;

	string tmp_name = "_action_var_float";

	if (regex_search(action, match, val)) {
		// Create new var
		tmp = this->cmd.new_var(tmp_name, EKWA_FLOAT);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Add value to var
		tmp = this->cmd.set_value(tmp_name, match.str(1),
			EKWA_FLOAT);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Write var to the buffer
		tmp = this->cmd.to_buffer(tmp_name);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Write buffer's data to var
		tmp = this->cmd.from_buffer_to(name);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Remove tmp var
		tmp = this->cmd.remove_var(tmp_name);
		list.insert(list.end(), tmp.begin(), tmp.end());
	}
	else if (regex_search(action, match, var)) {
		tvar = this->find_var(match.str(1));

		if (tvar.type != EKWA_FLOAT) {
			cout << "Error: 14.\n";
			exit(1);
		}
		// Write var to the buffer
		tmp = this->cmd.to_buffer(match.str(1));
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Write buffer's data to var
		tmp = this->cmd.from_buffer_to(name);
		list.insert(list.end(), tmp.begin(), tmp.end());
	}

	return list;
}

vector<unsigned char *> _ekwa_function::action_var_int(string name,
	string action)
{
	vector<unsigned char *> list, tmp;
	regex var("^([a-zA-Z0-9_]+)$");
	regex val("^([0-9]+)$");
	struct ekwa_var tvar;
	smatch match;

	string tmp_name = "_action_var_int";

	if (regex_search(action, match, val)) {
		// Create new var
		tmp = this->cmd.new_var(tmp_name, EKWA_INT);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Add value to var
		tmp = this->cmd.set_value(tmp_name, match.str(1),
			EKWA_INT);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Write var to the buffer
		tmp = this->cmd.to_buffer(tmp_name);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Write buffer's data to var
		tmp = this->cmd.from_buffer_to(name);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Remove tmp var
		tmp = this->cmd.remove_var(tmp_name);
		list.insert(list.end(), tmp.begin(), tmp.end());
	}
	else if (regex_search(action, match, var)) {
		tvar = this->find_var(match.str(1));

		if (tvar.type != EKWA_INT) {
			cout << "Error: 13.\n";
			exit(1);
		}
		// Write var to the buffer
		tmp = this->cmd.to_buffer(match.str(1));
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Write buffer's data to var
		tmp = this->cmd.from_buffer_to(name);
		list.insert(list.end(), tmp.begin(), tmp.end());
	}

	return list;
}

vector<unsigned char *> _ekwa_function::action_var_string(string name,
	string action)
{
	vector<unsigned char *> list, tmp;
	regex var("^([a-zA-Z0-9_]+)$");
	regex val("\"(.+?)\"$");
	struct ekwa_var tvar;
	smatch match;

	string tmp_name = "_action_var_string";

	if (regex_search(action, match, val)) {
		// Create new var
		tmp = this->cmd.new_var(tmp_name, EKWA_BYTES);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Add value to var
		tmp = this->cmd.set_value(tmp_name, match.str(1),
			EKWA_BYTES);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Write var to the buffer
		tmp = this->cmd.to_buffer(tmp_name);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Write buffer's data to var
		tmp = this->cmd.from_buffer_to(name);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Remove tmp var
		tmp = this->cmd.remove_var(tmp_name);
		list.insert(list.end(), tmp.begin(), tmp.end());
	}
	else if (regex_search(action, match, var)) {
		tvar = this->find_var(match.str(1));

		if (tvar.type != EKWA_BYTES) {
			cout << "Error: 9.\n";
			exit(1);
		}
		// Write var to the buffer
		tmp = this->cmd.to_buffer(match.str(1));
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Write buffer's data to var
		tmp = this->cmd.from_buffer_to(name);
		list.insert(list.end(), tmp.begin(), tmp.end());
	}

	return list;
}
//(\d+)
//(\d+[.]\d)
//"(.+?)"
//([+-])
void _ekwa_function::add_arg(struct ekwa_var var)
{
	for (auto &p : this->arguments) {
		if (p.name == var.name) {
			cout << "Error: 2.\n";
			exit(1);
		}
	}

	this->arguments.push_back(var);
}

bool _ekwa_function::var_exists(string name)
{
	for (auto &p : this->vars) {
		if (p.name == name) {
			return true;
		}
	}

	return false;
}

vector<unsigned char *> _ekwa_function::add_var(struct ekwa_var var)
{
	vector<unsigned char *> buff;

	for (auto &p : this->vars) {
		if (p.name == var.name) {
			cout << "Error: 3.\n";
			exit(1);
		}
	}

	this->vars.push_back(var);
	buff = this->cmd.new_var(var.name, var.type);

	return buff;
}

enum ekwa_types _ekwa_function::detect_type(string tp)
{
	enum ekwa_types type = EKWA_BYTES;

	if (tp == "int") {
		type = EKWA_INT;
	}

	if (tp == "float") {
		type = EKWA_FLOAT;
	}

	if (tp == "auto") {
		type = EKWA_CUSTOM;
	}

	return type;
}

struct ekwa_var _ekwa_function::find_var(string name)
{
	for (auto &p : this->vars) {
		if (p.name == name) {
			return p;
		}
	}

	cout << "Error: 8 - " << name << ".\n";
	exit(1);
}

void _ekwa_function::add_cmds(vector<unsigned char *> cmds)
{
	for (unsigned char *p : cmds) {
		this->print(p);
	}

	cout << "=============================================\n";

	this->instructions.insert(this->instructions.end(),
		cmds.begin(), cmds.end());
}

void _ekwa_function::print(unsigned char *list)
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