#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <queue>
#include <math.h>
#include "basic.h"
#include "util.h"
using namespace std;
//int conflict_flag;

int main(){
	

	vector<Node>& PI;
	vector<Node>& PO;
	vector<Node>& Gate;
	read_circuit
	//vector<GATE *> null;
	//Node PI_template  ={null,null,0,_PI,0,0,0};
	//Node PO_template  ={null,null,0,_PO,2,2147483647,0};
	//Node Gate_template={null,null,0,_NAND,2,65538,0};
	//conflict_flag=0;

	//PI.resize(2,PI_template);
	//PI[1].num=1;
	//PO.resize(1,PO_template);
	//PO[0].num=3;
	//Gate.resize(1,Gate_template);
	//Gate[0].num=2;
	
	//PI[0].fanout.push_back(&Gate[0]);
	//PI[1].fanout.push_back(&Gate[0]);
	//PO[0].fanin.push_back(&Gate[0]);
	//Gate[0].fanout.push_back(&PO[0]);
	//Gate[0].fanin.push_back(&PI[0]);
	//Gate[0].fanin.push_back(&PI[1]);
	
	print_map(PI,Gate,PO);
	simulation(PI);
	print_map(PI,Gate,PO);

	//Path p[2];
	//p[0].gates.push_back(&PI[0]);
	//p[0].gates.push_back(&Gate[0]);
	//p[0].gates.push_back(&PO[0]);
	//p[1].gates.push_back(&PI[0]);
	//p[1].gates.push_back(&Gate[0]);
	//p[1].gates.push_back(&PO[0]);
	
	return 0;
}
