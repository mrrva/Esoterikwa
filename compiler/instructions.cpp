
#include "compiler.hpp"

vector<unsigned char *> _ekwa_instructions::new_var(string name,
	enum ekwa_types type)
{
	unsigned char *cmd, token = EKWA_VAR;
	vector<unsigned char *> list;
	uint16_t len = name.length(), tp = 1;
	const char *cname = name.c_str();

	if (len == 0 || len > MAXBUFFER_LEN) {
		cout << "Error: 4.\n";
		exit(1);
	}

	cmd = new unsigned char[len + 100];
	memset(cmd, 0x00, len + 100);

	memcpy(cmd, &token, 1);
	memcpy(cmd + 1, &len, 2);
	memcpy(cmd + 3, cname, len);
	memcpy(cmd + 3 + len, &tp, 2);
	memcpy(cmd + 5 + len, &type, 1);

	list.push_back(cmd);

	return list;
}

vector<unsigned char *> _ekwa_instructions::set_value(string name,
	string value, enum ekwa_types type)
{
	unsigned char *buffer, token = EKWA_VAL, tp;
	uint16_t n_len = name.length(), v_len;
	const char *c_name = name.c_str(), *bval;
	vector<unsigned char *> list;
	float fval = 0;
	int ival = 0;

	if (n_len == 0 || n_len > MAXBUFFER_LEN) {
		cout << "Error: 5.\n";
		exit(1);
	}

	buffer = new unsigned char[20 + n_len + value.length()];
	tp = static_cast<unsigned char>(type);

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
				cout << "Error: 6.\n";
				exit(1);
			}

			memcpy(buffer + 5 + n_len, bval, v_len);
			if (bval && bval != nullptr)
				memcpy(buffer + 3 + n_len, &v_len, 2);
			break;
	}

	list.push_back(buffer);
	return list;
}

vector<unsigned char *> _ekwa_instructions::concat(string v1,
	string v2)
{
	uint16_t v1len = v1.length(), v2len = v2.length();
	vector<unsigned char *> list;
	unsigned char *cmd, token = EKWA_CAT;
	const char *v1_c = v1.c_str(), *v2_c = v2.c_str();

	if (v1len == 0 || v1len > MAXBUFFER_LEN || v2len == 0
		|| v2len > MAXBUFFER_LEN) {
		cout << "Error: 7.\n";
		exit(1);
	}

	cmd = new unsigned char[v2len + v1len + 100];

	memcpy(cmd, &token, 1);
	memcpy(cmd + 1, &v1len, 2);
	memcpy(cmd + 3, v1_c, v1len);
	memcpy(cmd + 3 + v1len, &v2len, 2);
	memcpy(cmd + 5 + v1len, v2_c, v2len);

	list.push_back(cmd);
	return list;
}

vector<unsigned char *> _ekwa_instructions::to_buffer(string name)
{
	unsigned char *cmd, token = EKWA_BUFF;
	const char *cname = name.c_str();
	vector<unsigned char *> list;
	uint16_t len = name.length();

	if (len == 0 || len > MAXBUFFER_LEN) {
		cout << "Error: 10.\n";
		exit(1);
	}

	cmd = new unsigned char[len + 100];
	memset(cmd, 0x00, len + 100);

	memcpy(cmd, &token, 1);
	memcpy(cmd + 1, &len, 2);
	memcpy(cmd + 3, cname, len);

	list.push_back(cmd);
	return list;
}

vector<unsigned char *> _ekwa_instructions::from_buffer_to(string name)
{
	unsigned char *cmd, token = EKWA_WRT;
	const char *cname = name.c_str();
	vector<unsigned char *> list;
	uint16_t len = name.length();

	if (len == 0 || len > MAXBUFFER_LEN) {
		cout << "Error: 11.\n";
		exit(1);
	}

	cmd = new unsigned char[len + 100];
	memset(cmd, 0x00, len + 100);

	memcpy(cmd, &token, 1);
	memcpy(cmd + 1, &len, 2);
	memcpy(cmd + 3, cname, len);

	list.push_back(cmd);
	return list;
}


vector<unsigned char *> _ekwa_instructions::remove_var(string name)
{
	unsigned char *cmd, token = EKWA_RMV;
	const char *cname = name.c_str();
	vector<unsigned char *> list;
	uint16_t len = name.length();

	if (len == 0 || len > MAXBUFFER_LEN) {
		cout << "Error: 12.\n";
		exit(1);
	}

	cmd = new unsigned char[len + 100];
	memset(cmd, 0x00, len + 100);

	memcpy(cmd, &token, 1);
	memcpy(cmd + 1, &len, 2);
	memcpy(cmd + 3, cname, len);

	list.push_back(cmd);
	return list;
}


vector<unsigned char *> _ekwa_instructions::to_arg(string arg)
{
	unsigned char *cmd, token = EKWA_ARG;
	const char *name = arg.c_str();
	vector<unsigned char *> list;
	uint16_t len = arg.length();

	if (len > MAXBUFFER_LEN || len == 0) {
		cout << "Error: 17.\n";
		exit(1);
	}

	cmd = new unsigned char[len + 100];
	memset(cmd, 0x00, len + 100);

	memcpy(cmd, &token, 1);
	memcpy(cmd + 1, &len, 2);
	memcpy(cmd + 3, name, len);

	list.push_back(cmd);
	return list;
}

vector<unsigned char *> _ekwa_instructions::call_function(string name)
{
	unsigned char *cmd, token = EKWA_CALL;
	const char *cname = name.c_str();
	vector<unsigned char *> list;
	uint16_t len = name.length();

	if (len > MAXBUFFER_LEN || len == 0) {
		cout << "Error: 18.\n";
		exit(1);
	}

	cmd = new unsigned char[len + 100];
	memset(cmd, 0x00, len + 100);

	memcpy(cmd, &token, 1);
	memcpy(cmd + 1, &len, 2);
	memcpy(cmd + 3, cname, len);

	list.push_back(cmd);
	return list;
}


vector<unsigned char *> _ekwa_instructions::plus(string v1,
	string v2)
{
	uint16_t v1len = v1.length(), v2len = v2.length();
	vector<unsigned char *> list;
	unsigned char *cmd, token = EKWA_ADD;
	const char *v1_c = v1.c_str(), *v2_c = v2.c_str();

	if (v1len == 0 || v1len > MAXBUFFER_LEN || v2len == 0
		|| v2len > MAXBUFFER_LEN) {
		cout << "Error: 23.\n";
		exit(1);
	}

	cmd = new unsigned char[v2len + v1len + 100];

	memcpy(cmd, &token, 1);
	memcpy(cmd + 1, &v1len, 2);
	memcpy(cmd + 3, v1_c, v1len);
	memcpy(cmd + 3 + v1len, &v2len, 2);
	memcpy(cmd + 5 + v1len, v2_c, v2len);

	list.push_back(cmd);
	return list;
}

vector<unsigned char *> _ekwa_instructions::minus(string v1,
	string v2)
{
	uint16_t v1len = v1.length(), v2len = v2.length();
	vector<unsigned char *> list;
	unsigned char *cmd, token = EKWA_SUB;
	const char *v1_c = v1.c_str(), *v2_c = v2.c_str();

	if (v1len == 0 || v1len > MAXBUFFER_LEN || v2len == 0
		|| v2len > MAXBUFFER_LEN) {
		cout << "Error: 28.\n";
		exit(1);
	}

	cmd = new unsigned char[v2len + v1len + 100];

	memcpy(cmd, &token, 1);
	memcpy(cmd + 1, &v1len, 2);
	memcpy(cmd + 3, v1_c, v1len);
	memcpy(cmd + 3 + v1len, &v2len, 2);
	memcpy(cmd + 5 + v1len, v2_c, v2len);

	list.push_back(cmd);
	return list;
}

vector<unsigned char *> _ekwa_instructions::if_body(string f1,
	string f2)
{
	uint16_t f1len = f1.length(), f2len = f2.length();
	vector<unsigned char *> list;
	unsigned char *cmd, token = EKWA_JMP;
	const char *f1_c = f1.c_str(), *f2_c = f2.c_str();

	if (f1len == 0 || f1len > MAXBUFFER_LEN || f2len == 0
		|| f2len > MAXBUFFER_LEN) {
		cout << "Error: 30.\n";
		exit(1);
	}

	cmd = new unsigned char[f1len + 200];
	memset(cmd, 0x00, f1len + 200);

	memcpy(cmd, &token, 1);
	memcpy(cmd + 1, &f1len, 2);
	memcpy(cmd + 3, f1_c, f1len);
	list.push_back(cmd);


	cmd = new unsigned char[f2len + 200];
	memset(cmd, 0x00, f2len + 200);

	memcpy(cmd, &token, 1);
	memcpy(cmd + 1, &f2len, 2);
	memcpy(cmd + 3, f2_c, f2len);
	list.push_back(cmd);

	cmd = new unsigned char[f2len + 200];
	memset(cmd, 0x00, f2len + 200);
	token = EKWA_FSET;

	memcpy(cmd, &token, 1);
	memcpy(cmd + 1, &f1len, 2);
	memcpy(cmd + 3, f1_c, f1len);
	list.push_back(cmd);

	return list;
}

vector<unsigned char *> _ekwa_instructions::comparing(string v1,
	string v2, enum ekwa_tokens tp)
{
	uint16_t v1len = v1.length(), v2len = v2.length();
	vector<unsigned char *> list;
	unsigned char *cmd, token = tp;
	const char *v1_c = v1.c_str(), *v2_c = v2.c_str();

	if (v1len == 0 || v1len > MAXBUFFER_LEN || v2len == 0
		|| v2len > MAXBUFFER_LEN) {
		cout << "Error: 32.\n";
		exit(1);
	}

	cmd = new unsigned char[v2len + v1len + 100];

	memcpy(cmd, &token, 1);
	memcpy(cmd + 1, &v1len, 2);
	memcpy(cmd + 3, v1_c, v1len);
	memcpy(cmd + 3 + v1len, &v2len, 2);
	memcpy(cmd + 5 + v1len, v2_c, v2len);

	list.push_back(cmd);
	return list;
}
