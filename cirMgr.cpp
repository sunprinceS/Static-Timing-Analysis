#include "cirMgr.h"
void cirMgr::print_map(){
	int i,size;
	cout<<"PIs:\n";
	size=this->PI.size();
	for(i=0;i<size;i++)
		print_node(this->PI[i]);

	cout<<"Gates:\n";
	size=this->Gate.size();
	for(i=0;i<size;i++)
		print_node(this->Gate[i]);
	
	cout<<"POs:\n";
	size=this->PO.size();
	for(i=0;i<size;i++)
		print_node(this->PO[i]);
	
}

void cirMgr::print_node(Node v){
	int i,size;
	if(v.type==2)
		cout<<"NAND";
	else if(v.type==3)
		cout<<"NOR";
	else if(v.type==0)
		cout<<"PI";
	else if(v.type==1)
		cout<<"PO";
	else 
		cout<<"UNKNOWN";
	
	cout<<"["<<v.num<<"]="<<v.value<<" @"<<v.arrival_time<<", ";
	cout<<"fanin:";
	size=v.fanin.size();
	for(i=0;i<size;i++)
		cout<<v.fanin[i]->num<<", ";
	
	cout<<"  fanout:";
	size=v.fanout.size();
	for(i=0;i<size;i++)
		cout<<v.fanout[i]->num<<", ";
	
	cout<<endl;
}

void cirMgr::simulation(){
	queue<Node*> q;
	Node* now;
	int i,size=this->PI.size();
	for(i=0;i<size;i++)
		q.push(&PI[i]);
		
	while(q.size()!=0){//calc value & arrival_time
		now=q.front();
		q.pop();
		size=now->fanout.size();
		for(i=0;i<size;i++){
			Node* son=now->fanout[i];
			if(son->type==1){
				son->value=now->value;
				son->arrival_time=now->arrival_time;
				continue;
			}
			else if(now->value==0&&son->type==2&&son->value==2){//NAND gate got 1st input 0
				son->value=1;
				son->arrival_time=now->arrival_time+1;
				q.push(son);
			}
			else if(now->value==1&&son->type==3&&son->value==2){//NOR gate got 1st input 1
				son->value=0;
				son->arrival_time=now->arrival_time+1;
				q.push(son);
			}
			else if(son->type==2&&son->value==2&&son->fanin[0]->value==1&&son->fanin[1]->value==1){//NAND get 2nd input 1
				son->value=0;
				son->arrival_time=now->arrival_time+1;
				q.push(son);
			}
			else if(son->type==3&&son->value==2&&son->fanin[0]->value==0&&son->fanin[1]->value==0){//NOR get 2nd input 0
				son->value=1;
				son->arrival_time=now->arrival_time+1;
				q.push(son);
			}
			else if(son->type == 4){
				son->value = 1 - now->value;
				son->arrival_time=now->arrival_time+1;
				q.push(son);
			}
		}
	}
}

bool cirMgr::path_is_true(Path p){//不傳進來能用指標只到整張圖?
	int i,size=p.gates.size();
	bool true_flag=(p.gates[0]->arrival_time==0&&p.gates[size-1]->arrival_time==size-1);
	
	for(i=1;i<size-1;i++){
		if(p.gates[i]->arrival_time==i){
			if(p.gates[i-1]->value==1&&p.gates[i]->value==1&&p.gates[i]->type==2){//input is not the NAND gate controlling value 
				true_flag=0;
				break;
			}
			else if(p.gates[i-1]->value==0&&p.gates[i]->value==0&&p.gates[i]->type==3){//input is not the NOR gate controlling value 
				true_flag=0;
				break;
			}
		}
		else{
			true_flag=0;
			break;
		}
	}
	return true_flag;
}

bool cirMgr::read_circuit(const string& fileName){
	vector<string> input;
	ifstream fin(fileName.c_str(),ios::in);
	string str;
	while(getline(fin,str)){
		input.push_back(str);
	}
	fin.close();
	handleInput();
	return true;
}

bool cirMgr::handleInput(){
	vector<GATE *> null;
	Node PI_template  ={null,null,0,_PI,0,0,0};
	Node PO_template  ={null,null,0,_PO,2,2147483647,0};
	Node Gate_template={null,null,0,_NAND,2,65538,0};
	this->PI.resize(2,PI_template);
	this->PI[1].num=1;
	this->PO.resize(1,PO_template);
	this->PO[0].num=3;
	this->Gate.resize(1,Gate_template);
	this->Gate[0].num=2;
	
	this->PI[0].fanout.push_back(&Gate[0]);
	this->PI[1].fanout.push_back(&Gate[0]);
	this->PO[0].fanin.push_back(&Gate[0]);
	this->Gate[0].fanout.push_back(&PO[0]);
	this->Gate[0].fanin.push_back(&PI[0]);
	this->Gate[0].fanin.push_back(&PI[1]);
	return true;
}
