#include <vector>
using namespace std;

vector<Node*> implication(Node*,int);
bool need_to_change_input(Node*,int);
bool path_assign(Path,vector<Node>);//0/1 for assign success/failed
void assign_input(Node*,int,int,int);
void clear_value(Node*,int);
int allowed_ti(Node*,bool);
