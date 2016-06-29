#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <queue>
#include <math.h>
#include "basic.h"
#include "cirMgr.h"
using namespace std;
//int conflict_flag;

int main(int argc, char** argv){
	cirMgr* cir_mgr = 0;
	cir_mgr->read_circuit(argv[1]);
	//conflict_flag=0;

	
	cir_mgr->print_map();
	cir_mgr->simulation();
	cir_mgr->print_map();

	//Path p[2];
	//p[0].gates.push_back(&PI[0]);
	//p[0].gates.push_back(&Gate[0]);
	//p[0].gates.push_back(&PO[0]);
	//p[1].gates.push_back(&PI[0]);
	//p[1].gates.push_back(&Gate[0]);
	//p[1].gates.push_back(&PO[0]);
	
	return 0;
}
