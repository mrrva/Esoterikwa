
#include "compiler.hpp"

string jmp_br = "";

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
	regex rg_10(rg_show);
	regex rg_5(rgcall_m);
	regex rg_8(rg_break);
	regex rg_7(rg_loop);
	regex rg_9(rg_exit);
	regex rg_6(rg_if);
	regex rg_11(rgvar_noinit_div);
	regex rg_12(rgvar_noinit_mod);
	regex rg_13(rgvar_noinit_mul);

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

		if (regex_search(line, match, rg_7)) {
			this->get_loop(ss, flag_n);
			continue;
		}

		if (regex_search(line, match, rg_2)) {
			this->action_var_noinit(match.str(1),
				match.str(2));
			continue;
		}

		if (regex_search(line, match, rg_8)) {
			this->set_jump();
			continue;
		}

		if (regex_search(line, match, rg_9)) {
			this->set_exit();
			continue;
		}

		if (regex_search(line, match, rg_10)) {
			this->action_show(match.str(1));
			continue;
		}

		if (regex_search(line, match, rg_11)) {
			this->action_var_noinit_div(match.str(1),
					match.str(2));
			continue;
		}

		if (regex_search(line, match, rg_12)) {
			this->action_var_noinit_mod(match.str(1),
					match.str(2));
			continue;
		}

		if (regex_search(line, match, rg_13)) {
			this->action_var_noinit_mul(match.str(1),
					match.str(2));
			continue;
		}
	}
}

void _ekwa_function::action_show(string var)
{
	vector<unsigned char *> list;

	if (!this->var_exists(var)) {
		cout << "Error: 35.\n";
		exit(1);
	}

	list = this->cmd.one_arg(var, EKWA_SHOW);
	this->add_cmds(list);
}

void _ekwa_function::set_exit(void)
{
	vector<unsigned char *> list;

	list = this->cmd.exit_s();
	this->add_cmds(list);
}

void _ekwa_function::set_jump(void)
{
	vector<unsigned char *> list, tmp;

	if (jmp_br.length() < 5) {
		cout << "Error: 33.\n";
		exit(1);
	}

	tmp = this->cmd.one_arg(jmp_br, EKWA_JMP);
	list.insert(list.end(), tmp.begin(), tmp.end());

	jmp_br = string();
	this->add_cmds(list);
}

void _ekwa_function::get_loop(stringstream &ss, size_t &fnum)
{
	vector<unsigned char *> list, tmp;
	string line, code(""), fn1, fn2;

	while (getline(ss, line, '\n')) {
		if (line[0] == '\t') {
			code += line.substr(1) + "\n";
			continue;
		}

		if (line[0] == '{') {
			continue;
		}

		if (line[0] == '}') {
			break;
		}
	}

	fn1 = "_" + this->name + "_flag_" + to_string(fnum++);
	fn2 = "_" + this->name + "_flag_" + to_string(fnum++);

	tmp = this->cmd.one_arg(fn1, EKWA_FSET);
	list.insert(list.end(), tmp.begin(), tmp.end());

	jmp_br = fn2;
	this->add_cmds(list);
	list.erase(list.begin(), list.end());
	// code to bytes ..
	this->code_to_instructions(code);

	tmp = this->cmd.loop_end(fn1, fn2);
	list.insert(list.end(), tmp.begin(), tmp.end());

	this->add_cmds(list);
}

void _ekwa_function::get_if(string var, string cond, string val,
	stringstream &ss, size_t &fnum)
{
	vector<unsigned char *> list, tmp;
	string line, code(""), fn1, fn2;
	enum ekwa_tokens tp = EKWA_IFE;
	struct ekwa_var tvar;
	smatch match;
	regex rg_flt("^([0-9]+[.][0-9]+)$");
	regex rg_var("^([a-zA-Z0-9_]+)$");
	regex rg_str("^[\"](.*)[\"]$");
	regex rg_int("^([0-9]+)$");
	string tmp_name = "_get_if_var";
	bool tmp_n = false;

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

		if (line[0] == '}') {
			break;
		}
	}

	tvar = this->find_var(var);

	if (cond == ">") {
		tp = EKWA_IFB;
	}

	if (cond == "<") {
		tp = EKWA_IFS;
	}

	if (cond == "!=") {
		tp = EKWA_IFNE;
	}

	if (regex_search(val, match, rg_int)) {
		tmp = this->cmd.new_var(tmp_name, EKWA_INT);
		list.insert(list.end(), tmp.begin(), tmp.end());

		tmp = this->cmd.set_value(tmp_name, val, EKWA_INT);
		list.insert(list.end(), tmp.begin(), tmp.end());

		tmp = this->cmd.comparing(var, tmp_name, tp);
		list.insert(list.end(), tmp.begin(), tmp.end());

		tmp_n = true;
	}
	else if (regex_search(val, match, rg_flt)) {
		tmp = this->cmd.new_var(tmp_name, EKWA_FLOAT);
		list.insert(list.end(), tmp.begin(), tmp.end());

		tmp = this->cmd.set_value(tmp_name, val, EKWA_FLOAT);
		list.insert(list.end(), tmp.begin(), tmp.end());

		tmp = this->cmd.comparing(var, tmp_name, tp);
		list.insert(list.end(), tmp.begin(), tmp.end());

		tmp_n = true;
	}
	else if (regex_search(val, match, rg_str)) {
		tmp = this->cmd.new_var(tmp_name, EKWA_BYTES);
		list.insert(list.end(), tmp.begin(), tmp.end());

		tmp = this->cmd.set_value(tmp_name, match.str(1),
			EKWA_BYTES);
		list.insert(list.end(), tmp.begin(), tmp.end());

		tmp = this->cmd.comparing(var, tmp_name, tp);
		list.insert(list.end(), tmp.begin(), tmp.end());

		tmp_n = true;
	}
	else if (regex_search(val, match, rg_var)) {
		if (!this->var_exists(val)) {
			cout << "Error: 32.\n";
			exit(1);
		}

		tmp = this->cmd.comparing(var, val, tp);
		list.insert(list.end(), tmp.begin(), tmp.end());
	}

	fn1 = "_" + this->name + "_flag_" + to_string(fnum++);
	fn2 = "_" + this->name + "_flag_" + to_string(fnum++);

	tmp = this->cmd.if_body(fn1, fn2);
	list.insert(list.end(), tmp.begin(), tmp.end());
	this->add_cmds(list);
	list.erase(list.begin(), list.end());
	// code to bytes ..
	this->code_to_instructions(code);

	tmp = this->cmd.one_arg(fn2, EKWA_FSET);
	list.insert(list.end(), tmp.begin(), tmp.end());

	if (tmp_n) {
		tmp = this->cmd.one_arg(tmp_name, EKWA_RMV);
		list.insert(list.end(), tmp.begin(), tmp.end());
	}

	this->add_cmds(list);
}

void _ekwa_function::action_funcm_call(string name,
	string arg_str)
{
	vector<string> args = this->get_fnargs(arg_str);
	vector<unsigned char *> list, tmp;

	for (auto &p : args) {
		tmp = this->cmd.one_arg(p, EKWA_ARG);
		list.insert(list.end(), tmp.begin(), tmp.end());
	}

	tmp = this->cmd.one_arg(name, EKWA_CALL);
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
		buff = this->action_var_int(name, action, EKWA_SUB);
		break;

	case EKWA_FLOAT:
		buff = this->action_var_float(name, action, EKWA_SUB);
		break;

	default:
		cout << "Error: 26.\n";
		exit(1);
	}

	this->add_cmds(buff);
}

void _ekwa_function::action_var_noinit_div(string name,
	string action)
{
	struct ekwa_var tvar = this->find_var(name);
	vector<unsigned char *> buff;

	if (!this->var_exists(name)) {
		cout << "Error: 40.\n";
		exit(1);
	}

	switch (tvar.type) {
	case EKWA_INT:
		buff = this->action_var_int(name, action, EKWA_DIV);
		break;

	case EKWA_FLOAT:
		buff = this->action_var_float(name, action, EKWA_DIV);
		break;

	default:
		cout << "Error: 41.\n";
		exit(1);
	}

	this->add_cmds(buff);
}




void _ekwa_function::action_var_noinit_mul(string name,
	string action)
{
	struct ekwa_var tvar = this->find_var(name);
	vector<unsigned char *> buff;

	if (!this->var_exists(name)) {
		cout << "Error: 40.\n";
		exit(1);
	}

	switch (tvar.type) {
	case EKWA_INT:
		buff = this->action_var_int(name, action, EKWA_MUL);
		break;

	case EKWA_FLOAT:
		buff = this->action_var_float(name, action, EKWA_MUL);
		break;

	default:
		cout << "Error: 41.\n";
		exit(1);
	}

	this->add_cmds(buff);
}

















void _ekwa_function::action_var_noinit_mod(string name,
	string action)
{
	struct ekwa_var tvar = this->find_var(name);
	vector<unsigned char *> buff;

	if (!this->var_exists(name)) {
		cout << "Error: 44.\n";
		exit(1);
	}

	switch (tvar.type) {
	case EKWA_INT:
		buff = this->action_var_int(name, action, EKWA_MOD);
		break;

	default:
		cout << "Error: 45.\n";
		exit(1);
	}

	this->add_cmds(buff);
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
		buff = this->action_var_string(name, action, true);
		break;

	case EKWA_INT:
		buff = this->action_var_int(name, action, EKWA_ADD);
		break;

	case EKWA_FLOAT:
		buff = this->action_var_float(name, action, EKWA_ADD);
		break;

	default:
		cout << "Error: 24.\n";
		exit(1);
	}

	this->add_cmds(buff);
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
		buff = this->action_var_string(name, action, false);
		break;

	case EKWA_INT:
		buff = this->action_var_int(name, action, 0xff);
		break;

	case EKWA_FLOAT:
		buff = this->action_var_float(name, action, 0xff);
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
		buff = this->action_var_string(name, action, false);
		break;

	case EKWA_INT:
		buff = this->action_var_int(name, action, 0xff);
		break;

	case EKWA_FLOAT:
		buff = this->action_var_float(name, action, 0xff);
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
		tmp = this->cmd.one_arg(match.str(1), EKWA_BUFF);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Write buffer's data to var
		tmp = this->cmd.one_arg(name, EKWA_WRT);
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
			tmp = this->cmd.one_arg(p, EKWA_ARG);
			list.insert(list.end(), tmp.begin(), tmp.end());
		}
		// Call function
		tmp = this->cmd.one_arg(match.str(1), EKWA_CALL);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Write buffer's data to var
		tmp = this->cmd.one_arg(name, EKWA_WRT);
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
	string action, unsigned char token)
{
	vector<unsigned char *> list, tmp;
	regex val("^([0-9]+[.][0-9]+)$");
	regex var("^([a-zA-Z0-9_]+)$");
	struct ekwa_var tvar;
	smatch match;

	string tmp_name = "_action_var_float_";

	if (regex_search(action, match, val)) {
		// Create new var
		tmp = this->cmd.new_var(tmp_name, EKWA_FLOAT);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Add value to var
		tmp = this->cmd.set_value(tmp_name, match.str(1),
			EKWA_FLOAT);
		list.insert(list.end(), tmp.begin(), tmp.end());

		if (token == 0xff) {
			// Write var to the buffer
			tmp = this->cmd.one_arg(tmp_name, EKWA_BUFF);
			list.insert(list.end(), tmp.begin(), tmp.end());
		}
		else {
			// Call token
			tmp = this->cmd.two_args(name, tmp_name, token);
			list.insert(list.end(), tmp.begin(), tmp.end());
		}

		// Write buffer's data to var
		tmp = this->cmd.one_arg(name, EKWA_WRT);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Remove tmp var
		tmp = this->cmd.one_arg(tmp_name, EKWA_RMV);
		list.insert(list.end(), tmp.begin(), tmp.end());
	}
	else if (regex_search(action, match, var)) {
		tvar = this->find_var(match.str(1));

		if (tvar.type != EKWA_FLOAT) {
			cout << "Error: 14.\n";
			exit(1);
		}

		if (token == 0xff) {
			// Write var to the buffer
			tmp = this->cmd.one_arg(match.str(1), EKWA_BUFF);
			list.insert(list.end(), tmp.begin(), tmp.end());
		}
		else {
			// Call token
			tmp = this->cmd.two_args(name, match.str(1), token);
			list.insert(list.end(), tmp.begin(), tmp.end());
		}

		// Write buffer's data to var
		tmp = this->cmd.one_arg(name, EKWA_WRT);
		list.insert(list.end(), tmp.begin(), tmp.end());
	}

	return list;
}




































vector<unsigned char *> _ekwa_function::action_var_int(string name,
	string action, unsigned char token)
{
	vector<unsigned char *> list, tmp;
	regex var("^([a-zA-Z0-9_]+)$");
	regex val("^([0-9]+)$");
	struct ekwa_var tvar;
	smatch match;

	string tmp_name = "_action_var_int_";

	if (regex_search(action, match, val)) {
		// Create new var
		tmp = this->cmd.new_var(tmp_name, EKWA_INT);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Add value to var
		tmp = this->cmd.set_value(tmp_name, match.str(1),
			EKWA_INT);
		list.insert(list.end(), tmp.begin(), tmp.end());

		if (token == 0xff) {
			// Write var to the buffer
			tmp = this->cmd.one_arg(tmp_name, EKWA_BUFF);
			list.insert(list.end(), tmp.begin(), tmp.end());
		}
		else {
			// Call token
			tmp = this->cmd.two_args(name, tmp_name, token);
			list.insert(list.end(), tmp.begin(), tmp.end());
		}

		// Write buffer's data to var
		tmp = this->cmd.one_arg(name, EKWA_WRT);
		list.insert(list.end(), tmp.begin(), tmp.end());
		// Remove tmp var
		tmp = this->cmd.one_arg(tmp_name, EKWA_RMV);
		list.insert(list.end(), tmp.begin(), tmp.end());
	}
	else if (regex_search(action, match, var)) {
		tvar = this->find_var(match.str(1));

		if (tvar.type != EKWA_INT) {
			cout << "Error: 13.\n";
			exit(1);
		}

		if (token == 0xff) {
			// Write var to the buffer
			tmp = this->cmd.one_arg(match.str(1), EKWA_BUFF);
			list.insert(list.end(), tmp.begin(), tmp.end());
		}
		else {
			// Call token
			tmp = this->cmd.two_args(name, match.str(1), token);
			list.insert(list.end(), tmp.begin(), tmp.end());
		}

		// Write buffer's data to var
		tmp = this->cmd.one_arg(name, EKWA_WRT);
		list.insert(list.end(), tmp.begin(), tmp.end());
	}

	return list;
}











vector<unsigned char *> _ekwa_function::action_var_string(string name,
	string action, bool cat)
{
	vector<unsigned char *> list, tmp;
	regex var("^([a-zA-Z0-9_]+)$");
	regex val("\"(.+?)\"$");
	struct ekwa_var tvar;
	smatch match;

	string tmp_name = "_action_var_string_";

	if (regex_search(action, match, val)) {
		// Create new var
		tmp = this->cmd.new_var(tmp_name, EKWA_BYTES);
		list.insert(list.end(), tmp.begin(), tmp.end());

		// Add value to var
		tmp = this->cmd.set_value(tmp_name, match.str(1),
			EKWA_BYTES);
		list.insert(list.end(), tmp.begin(), tmp.end());
		///////////////////////////////////////////////////
		if (cat == false) {
			// Write var to the buffer
			tmp = this->cmd.one_arg(tmp_name, EKWA_BUFF);
			list.insert(list.end(), tmp.begin(), tmp.end());
			// Write buffer's data to var
			tmp = this->cmd.one_arg(name, EKWA_WRT);
			list.insert(list.end(), tmp.begin(), tmp.end());
		}
		else {
			// Call concat token
			tmp = this->cmd.two_args(name, tmp_name, EKWA_CAT);
			list.insert(list.end(), tmp.begin(), tmp.end());
		}
		///////////////////////////////////////////////////
		// Remove tmp var
		tmp = this->cmd.one_arg(tmp_name, EKWA_RMV);
		list.insert(list.end(), tmp.begin(), tmp.end());
	}
	else if (regex_search(action, match, var)) {
		tvar = this->find_var(match.str(1));

		if (tvar.type != EKWA_BYTES) {
			cout << "Error: 9.\n";
			exit(1);
		}

		if (cat == false) {
			// Write var to the buffer
			tmp = this->cmd.one_arg(match.str(1), EKWA_BUFF);
			list.insert(list.end(), tmp.begin(), tmp.end());
			// Write buffer's data to var
			tmp = this->cmd.one_arg(name, EKWA_WRT);
			list.insert(list.end(), tmp.begin(), tmp.end());
		}
		else {
			// Call concat token
			tmp = this->cmd.two_args(name, match.str(1), EKWA_CAT);
			list.insert(list.end(), tmp.begin(), tmp.end());
		}
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

	if (len == 0) {
		cout << endl;
		return;
	}

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

void _ekwa_function::write_file(string file)
{
	unsigned char *buffer;
	enum ekwa_tokens token;
	uint16_t len = 0, len2 = 0;
	ofstream write_s(file, ofstream::binary);

	if (!write_s.is_open()) {
		cout << "Can't open file for writing bytecode.\n";
		exit(1);
	}

	for (unsigned char *p : this->instructions) {
		this->print(p);
		// token
		memcpy(&token, p, 1);
		write_s.write(reinterpret_cast<char *>(&token), 1);

		// length
		memcpy(&len, p + 1, sizeof(uint16_t));
		write_s.write(reinterpret_cast<char *>(p + 1), sizeof(uint16_t));

		if (len == 0) {
			this->print(p);
			continue;
		}

		buffer = new unsigned char[len + 1];

		// copy & write data
		memcpy(buffer, p + 3, len);
		write_s.write(reinterpret_cast<char *>(buffer), len);

		// get length of second arg
		memcpy(&len2, p + 3 + len, sizeof(uint16_t));
		write_s.write(reinterpret_cast<char *>(p + 3 + len), sizeof(uint16_t));

		delete[] buffer;

		if (len2 == 0) {
			continue;
		}

		buffer = new unsigned char[len2 + 1];

		// copy & write data
		memcpy(buffer, p + 5 + len, len2);
		write_s.write(reinterpret_cast<char *>(buffer), len2);

		delete[] buffer;
	}

	write_s.close();
}