#include "util.h"
void print_map(vector<Node> PI,vector<Node> Gate,vector<Node> PO){
	int i,size;
	cout<<"PIs:\n";
	size=PI.size();
	for(i=0;i<size;i++)
		print_node(PI[i]);

	cout<<"Gates:\n";
	size=Gate.size();
	for(i=0;i<size;i++)
		print_node(Gate[i]);
	
	cout<<"POs:\n";
	size=PO.size();
	for(i=0;i<size;i++)
		print_node(PO[i]);
	
}

void print_node(Node v){
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


void simulation(vector<Node> PI){
	queue<Node*> q;
	Node* now;
	int i,size=PI.size();
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
				son->arrival_time=now->arrival_time+1;
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
		}
	}
}

bool path_is_true(Path p){//不傳進來能用指標只到整張圖?
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
