#include <vector>
#include <iostream>
#include <sstream>
#include <cstring>
#include <string>
#include <queue>

#include "basic.h"
using namespace std;

class cirMgr
{
public:
	cirMgr();
	~cirMgr();
	void simulation();
	bool path_is_true(Path);
	void print_map();
	void print_node(Node);
	bool read_circuit(const string&);
	bool handleInput();

private:
	vector<Node> PI;
	vector<Node> PO;
	vector<Node> Gate;
};
