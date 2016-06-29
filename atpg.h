#include <vector>
using namespace std;

vector<Node*> implication(Node*,int);
bool need_to_change_input(Node*,int);
void path_assign(Path);
void assign_input(Node*,int,int,int);
void clear_value(Node*,int);
int allowed_ti(Node*);
