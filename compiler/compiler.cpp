
#include "compiler.hpp"

int main(int argc, char *argv[])
{
	//vector<_ekwa_function> f_list;
	struct txtfunction one;
	regex r_funcs(rgfns);
	regex rpls("[\n][\t]");
	_ekwa_instructions do_cmd;
	_ekwa_function *f_class;
	stringstream ss;
	ifstream fs;
	smatch match;
	string text;

	if (argc < 2) {
		cout << "Error: Haven't input file.\n";
		return 1;
	}

	fs = ifstream(string(argv[1]));

	if (!fs.is_open()) {
		cout << "Error: Haven't input file.\n";
		return 1;
	}

	ss << fs.rdbuf();
	text = ss.str();

	while (regex_search(text, match, r_funcs)) {
		one.body = match.str(3).substr(0, match.str(3).length() - 1);
		one.body = regex_replace(one.body, rpls, "\n");
		one.name = match.str(1);

		f_class = new _ekwa_function(one, do_cmd);
		f_class->write_file("./instructions");
		text = match.suffix();
	}
}


//fn ([0-9a-zA-Z_]+)$\n^{$([\s\S]+?^}$)