#ifndef BASIC_H
#define BASIC_H

#include <iostream>
#include <vector>
using namespace std;

enum GateType { 
	_PI = 0,
	_PO = 1,
	_NAND = 2,
	_NOR = 3,
	_NOT = 4,

	TOT_GATE_TYPE
};

typedef struct GATE{
	vector<GATE *> fanin,fanout;
	int num;
	GateType type;//0:PI, 1:PO, 2:NAND, 3:NOR
	int value;//0,1,x->0,1,2
	int arrival_time;
	int imply_level;
} Node;

typedef struct PATH{
	vector<GATE* > gates;//[0]->[n]=PI->PO
} Path;

#endif  //BASIC_H
