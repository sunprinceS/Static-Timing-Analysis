#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <queue>
#include <math.h>
using namespace std;
int conflict_flag;

typedef struct GATE{
	vector<GATE *> fanin,fanout;
	int num;
	int type;//0:PI, 1:PO, 2:NAND, 3:NOR
	int value;//0,1,x->0,1,2
	int arrival_time;
	int imply_level;
} Node;
typedef struct PATH{
	vector<GATE *> gates;//[0]->[n]=PI->PO
} Path;


void print_map(vector<Node>,vector<Node>,vector<Node>);
void print_node(Node);

void simulation(vector<Node>);
bool path_is_true(Path);

vector<Node*> implication(Node*,int);
bool need_to_change_input(Node*,int);
bool path_assign(Path,vector<Node>);//0/1 for assign success/failed
void assign_input(Node*,int,int,int);
void clear_value(Node*,int);
int allowed_ti(Node*,bool);

int main(){
	vector<Node> PI;
	vector<Node> PO;
	vector<Node> Gate;
	vector<GATE *> null;
	Node PI_template  ={null,null,0,0,0,0,0};
	Node PO_template  ={null,null,0,1,2,2147483647,0};
	Node Gate_template={null,null,0,2,2,65538,0};
	int n,i;
//	cin>>n;
//	for(i=0;i<n;i++){
//		
//		
//	}
	conflict_flag=0;

	//(0NAND1)=3;
	PI.resize(3,PI_template);
	PI[0].value=2;
	PI[1].num=1;
	PI[1].value=2;
	PI[2].num=2;
	PI[2].value=1;
	PO.resize(1,PO_template);
	PO[0].num=6;
	Gate.resize(3,Gate_template);
	Gate[0].num=3;
	Gate[1].num=4;
	Gate[1].type=4;
	Gate[2].num=5;
	Gate[2].type=3;
		
	//print_map(PI,Gate,PO);
	
	PI[0].fanout.push_back(&Gate[0]);
	PI[1].fanout.push_back(&Gate[0]);
	PI[2].fanout.push_back(&Gate[2]);
	PO[0].fanin.push_back(&Gate[2]);
	Gate[0].fanout.push_back(&Gate[1]);
	Gate[0].fanin.push_back(&PI[0]);
	Gate[0].fanin.push_back(&PI[1]);
	Gate[1].fanout.push_back(&Gate[2]);
	Gate[1].fanin.push_back(&Gate[0]);
	Gate[2].fanout.push_back(&PO[0]);
	Gate[2].fanin.push_back(&Gate[1]);
	Gate[2].fanin.push_back(&PI[2]);
	
//	print_map(PI,Gate,PO);
	simulation(PI);
	print_map(PI,Gate,PO);
	Path p[3];
	p[0].gates.push_back(&PI[0]);
	p[0].gates.push_back(&Gate[0]);
	p[0].gates.push_back(&Gate[1]);
	p[0].gates.push_back(&Gate[2]);
	p[0].gates.push_back(&PO[0]);
	p[1].gates.push_back(&PI[1]);
	p[1].gates.push_back(&Gate[0]);
	p[1].gates.push_back(&Gate[1]);
	p[1].gates.push_back(&Gate[2]);
	p[1].gates.push_back(&PO[0]);
	p[2].gates.push_back(&PI[2]);
	p[2].gates.push_back(&Gate[2]);
	p[2].gates.push_back(&PO[0]);
	bool pt[3];
	pt[0]=path_is_true(p[0]);
	pt[1]=path_is_true(p[1]);
	pt[2]=path_is_true(p[2]);
//	cout<<"path0:"<<pt[0]<<"\n";
//	cout<<"path1:"<<pt[1]<<"\n";
//	cout<<"path2:"<<pt[2]<<"\n";
	////////////////test clear_value //////////////////////////
	
	PI[0].imply_level=1;
	PI[1].imply_level=1;
	PI[2].imply_level=1;
	Gate[0].imply_level=1;
	Gate[1].imply_level=1;
	Gate[2].imply_level=1;
	PO[0].imply_level=1;
	clear_value(&PO[0],1);
	print_map(PI,Gate,PO);
	cout<<"START JIZZ!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
	path_assign(p[0],PI);
	print_map(PI,Gate,PO);
	
	/////////////////////test allowed_ti ////////////////////////////////////
	/*
	cout<<"allow t[i] for given value.\n";
	cout<<"PI[0]:"<<allowed_ti(&PI[0],1)<<endl;
	cout<<"PI[1]:"<<allowed_ti(&PI[1],1)<<endl;
	cout<<"PI[2]:"<<allowed_ti(&PI[2],1)<<endl;
	cout<<"Gate[0]:"<<allowed_ti(&Gate[0],1)<<endl;
	cout<<"Gate[1]:"<<allowed_ti(&Gate[1],1)<<endl;
	cout<<"Gate[2]:"<<allowed_ti(&Gate[2],1)<<endl;
	cout<<"PI[0]:"<<allowed_ti(&PO[0],1)<<endl;
	*/
	/////////////////////test assign_input ////////////////////////////////////
	/*
	assign_input(&PO[0],0,0,4);
	print_map(PI,Gate,PO);
	assign_input(&Gate[2],0,0,3);
	print_map(PI,Gate,PO);
	assign_input(&Gate[1],0,0,2);
	print_map(PI,Gate,PO);
	assign_input(&Gate[0],0,0,1);
	print_map(PI,Gate,PO);
	clear_value(&Gate[0],1);
	print_map(PI,Gate,PO);
	assign_input(&Gate[0],0,1,1);
	print_map(PI,Gate,PO);
	*/
	
	
	return 0;
}

bool need_to_change_input(Node* v,int level){//level=p["i"] in path, v belongs path, so we check need_to_change_input only when we already assign inputs,and output of it ;
	queue<Node*> q;
	int i,size,l=level,f0size=v->fanin[0]->fanout.size(),f1size;
	q.push(v->fanin[0]);
	for(i=0;i<f0size;i++){
		if(v->fanin[0]->fanout[i]!=v){
			q.push(v->fanin[0]->fanout[i]);
		}
	}
	if(v->type==2||v->type==3){
		f1size=v->fanin[1]->fanout.size();
		q.push(v->fanin[1]);
		for(i=0;i<f1size;i++){
			if(v->fanin[1]->fanout[i]!=v){
				q.push(v->fanin[1]->fanout[i]);
			}
		}
	}
	conflict_flag=0;
	while(q.size()!=0){
		cout<<"imply level:"<<l<<",node to imply:";
		print_node(*q.front());
		vector<Node*> newq=implication(q.front(),l);

		if(conflict_flag==1)
			return 1;
		q.pop();
		size=newq.size();
		cout<<"(@check)Add"<<size<<"node to queue\n";
		for(i=0;i<size;i++){
			q.push(newq[i]);
		}
	}
	return 0;
}

bool path_assign(Path p,vector<Node> PI){//need to clear imply_level of all gates
	int i,size=p.gates.size();
	vector<int> t;
	t.resize(size,0);
	
	for(i=size-1;i>0;i--){
		print_node(*p.gates[i]);
		
		if(i==size-1&&t[i]==2)
			return 0;
		
		
		if(p.gates[i-1]->value!=2){//current gate value is already assigned by previous(nearer to PO) gate
			continue;
		}
		clear_value(p.gates[i],i);
		if(t[i]>=allowed_ti(p.gates[i],1)){//the gate has tried all legal input assignment, so go back to previous assignment and try to change it
			t[i]=0;
			int j;
			for(j=1;i+j<size-1;j++){//for continuous imply_level we find the first imply gate(此gate是不同assignment的分歧點,之後的gate都是受此gate的assignment而imply出值)
				if(p.gates[i+j]->imply_level!=p.gates[i+j+1]->imply_level){
					break;
				}
			}
			i+=j+1;
		}
		if(p.gates[i]->fanin[0]==p.gates[i-1])//target path is current gate's fanin[0]->set prefer=0
			assign_input(p.gates[i],0,t[i],i);
		else//target path is current gate's fanin[1]->set prefer=1
			assign_input(p.gates[i],1,t[i],i);
		cout<<"Path gate "<<i<<" with t[i]="<<t[i]<<endl;
		print_node(*p.gates[i]);
			
		t[i]++;
		if(need_to_change_input(p.gates[i],i)){//meet conflict, change another input assignment(by keeping i the same for next loop)
			clear_value(p.gates[i],size-i);
			i+=1;
			cout<<"Need to change\n";
		}
		else 
			cout<<"No need to change\n";
		
		if(i==1){
			simulation(PI);//simulation發現爛掉，回朔前一次assignment分岐
			if(path_is_true(p)==0){
				int j;
				for(j=0;i+j<size-1;j++){
					if(p.gates[i+j]->imply_level!=p.gates[i+j+1]->imply_level){
						break;
					}
				}
				i+=j+1;
			}
		}
	}
	return 1;
}

vector<Node*> implication(Node* v,int level){
	vector<Node*> totest;
	//PI/PO dont need to implication
	cout<<"implying node with level="<<level<<":\n";
	print_node(*v);
	
	if(v->type==0||v->type==1){
		cout<<"No imply for PI/PO\n";
		return totest;
	}
		//check conflict
	if(v->type==2){
		if((v->fanin[1]->value==0||v->fanin[0]->value==0)&&v->value==0){
			conflict_flag=1;
			return totest;
		}
		if((v->fanin[1]->value==1&&v->fanin[0]->value==1)&&v->value==1){
			conflict_flag=1;
			return totest;
		}
	}
	if(v->type==3){
		if((v->fanin[1]->value==0&&v->fanin[0]->value==0)&&v->value==0){
			conflict_flag=1;
			return totest;
		}	
		if((v->fanin[1]->value==1||v->fanin[0]->value==1)&&v->value==1){
			conflict_flag=1;
			return totest;
		}
	}
	
	if(v->fanin[0]->value==v->value&&v->type==4&&(v->value==0||v->value==1)){
		conflict_flag=1;
		return totest;
	}
	cout<<"no conflict\n";
	//if find some implication, check if the new known value wire will cause other implication
	int i,size;
	if(v->type==2){//NAND
		if(v->value==2){
			if(v->fanin[0]->value==0&&v->fanin[1]->value!=0){
				v->value=1;
				v->imply_level=level;
				//v->arrival_time=v->fanin[0]->arrival_time;
				size=v->fanout.size();
				for(i=0;i<size;i++)
					totest.push_back(v->fanout[i]);
			}
			else if(v->fanin[1]->value==0&&v->fanin[0]->value!=0){
				v->value=1;
				v->imply_level=level;
				//v->arrival_time=v->fanin[1]->arrival_time;
				size=v->fanout.size();
				for(i=0;i<size;i++)
					totest.push_back(v->fanout[i]);
			}
			else if(v->fanin[1]->value==0&&v->fanin[0]->value==0){
				v->value=1;
				v->imply_level=level;
				//v->arrival_time=min(v->fanin[1]->arrival_time, v->fanin[0]->arrival_time);
				size=v->fanout.size();
				for(i=0;i<size;i++)
					totest.push_back(v->fanout[i]);
			}
			else if(v->fanin[1]->value==1&&v->fanin[0]->value==1){
				v->value=0;
				v->imply_level=level;
				//v->arrival_time=max(v->fanin[1]->arrival_time, v->fanin[0]->arrival_time);	
				size=v->fanout.size();
				for(i=0;i<size;i++)
					totest.push_back(v->fanout[i]);
			}
			else{
				cout<<"Nothing to imply1\n";
			}
		}
		else if(v->value==0){
			if(v->fanin[0]->value!=2&&v->fanin[1]->value!=2){
				cout<<"Nothing to imply2\n";
			}
			if(v->fanin[0]->value==2){
				v->fanin[0]->value=1;
				v->fanin[0]->imply_level=level;
				totest.push_back(v->fanin[0]);
				size=v->fanin[0]->fanout.size();
				for(i=0;i<size;i++){
					if(v->fanin[0]->fanout[i]!=v)
						totest.push_back(v->fanin[0]->fanout[i]);
				}
			}
			if(v->fanin[1]->value==2){
				v->fanin[1]->value=1;
				v->fanin[1]->imply_level=level;
				totest.push_back(v->fanin[1]);
				size=v->fanin[1]->fanout.size();
				for(i=0;i<size;i++){
					if(v->fanin[1]->fanout[i]!=v)
						totest.push_back(v->fanin[1]->fanout[i]);
				}
			}
		}
		else if(v->value==1){
			if(v->fanin[0]->value==1&&v->fanin[1]->value==2){
				v->fanin[1]->value=0;
				v->fanin[1]->imply_level=level;
				totest.push_back(v->fanin[1]);
				size=v->fanin[1]->fanout.size();
				for(i=0;i<size;i++){
					if(v->fanin[1]->fanout[i]!=v)
						totest.push_back(v->fanin[1]->fanout[i]);
				}
			}
			else if(v->fanin[0]->value==2&&v->fanin[1]->value==1){
				v->fanin[0]->value=0;
				v->fanin[0]->imply_level=level;
				totest.push_back(v->fanin[0]);
				size=v->fanin[0]->fanout.size();
				for(i=0;i<size;i++){
					if(v->fanin[0]->fanout[i]!=v)
						totest.push_back(v->fanin[0]->fanout[i]);
				}
			}
			else{
				cout<<"Nothing to imply3\n";
			}
		}
	}
	else if(v->type==4){//NOT
		cout<<"implying NOT gate\n";
		if(v->value==2&&v->fanin[0]->value!=2){
			v->value=1-v->fanin[0]->value;
			v->imply_level=level;
			size=v->fanout.size();
			for(i=0;i<size;i++)
				totest.push_back(v->fanout[i]);
			cout<<"add"<<totest.size()<<"node to queue\n";
		}
		else if(v->value!=2&&v->fanin[0]->value==2){
			v->fanin[0]->value=1-v->value;
			v->fanin[0]->imply_level=level;
			totest.push_back(v->fanin[0]);
			size=v->fanin[0]->fanout.size();
			for(i=0;i<size;i++){
				if(v->fanin[0]->fanout[i]!=v)
					totest.push_back(v->fanin[0]->fanout[i]);
			}
			cout<<"add"<<totest.size()<<"node to queue\n";
		}
		else{
			cout<<"Nothing to imply4,";
			print_node(*v->fanin[0]);
		}
	}
	else if(v->type==3){//NOR
		if(v->value==2){
			if(v->fanin[0]->value==1&&v->fanin[1]->value!=1){
				v->value=0;
				v->imply_level=level;
				//v->arrival_time=v->fanin[0]->arrival_time;
				size=v->fanout.size();
				for(i=0;i<size;i++)
					totest.push_back(v->fanout[i]);
			}
			else if(v->fanin[1]->value==1&&v->fanin[0]->value!=1){
				v->value=0;
				v->imply_level=level;
				//v->arrival_time=v->fanin[1]->arrival_time;
				size=v->fanout.size();
				for(i=0;i<size;i++)
					totest.push_back(v->fanout[i]);
			}
			else if(v->fanin[1]->value==1&&v->fanin[0]->value==1){
				v->value=0;
				v->imply_level=level;
				//v->arrival_time=min(v->fanin[1]->arrival_time, v->fanin[0]->arrival_time);
				size=v->fanout.size();
				for(i=0;i<size;i++)
					totest.push_back(v->fanout[i]);
			}
			else if(v->fanin[1]->value==0&&v->fanin[0]->value==0){
				v->value=1;
				v->imply_level=level;
				//v->arrival_time=max(v->fanin[1]->arrival_time, v->fanin[0]->arrival_time);	
				size=v->fanout.size();
				for(i=0;i<size;i++)
					totest.push_back(v->fanout[i]);
			}
			else{
				cout<<"Nothing to imply5\n";
			}
		}
		else if(v->value==1){
			if(v->fanin[0]->value!=2&&v->fanin[1]->value!=2){
				cout<<"Nothing to imply6\n";
			}
			if(v->fanin[0]->value==2){
				v->fanin[0]->value=0;
				v->fanin[0]->imply_level=level;
				totest.push_back(v->fanin[0]);
				size=v->fanin[0]->fanout.size();
				for(i=0;i<size;i++){
					if(v->fanin[0]->fanout[i]!=v)
						totest.push_back(v->fanin[0]->fanout[i]);
				}
			}
			if(v->fanin[1]->value==2){
				v->fanin[1]->value=0;
				v->fanin[1]->imply_level=level;
				totest.push_back(v->fanin[1]);
				size=v->fanin[1]->fanout.size();
				for(i=0;i<size;i++){
					if(v->fanin[1]->fanout[i]!=v)
						totest.push_back(v->fanin[1]->fanout[i]);
				}
			}
		}
		else if(v->value==0){
			if(v->fanin[0]->value==0&&v->fanin[1]->value==2){
				v->fanin[1]->value=1;
				v->fanin[1]->imply_level=level;
				totest.push_back(v->fanin[1]);
				size=v->fanin[1]->fanout.size();
				for(i=0;i<size;i++){
					if(v->fanin[1]->fanout[i]!=v)
						totest.push_back(v->fanin[1]->fanout[i]);
				}
			}
			else if(v->fanin[0]->value==2&&v->fanin[1]->value==0){
				v->fanin[0]->value=1;
				v->fanin[0]->imply_level=level;
				totest.push_back(v->fanin[0]);
				size=v->fanin[0]->fanout.size();
				for(i=0;i<size;i++){
					if(v->fanin[0]->fanout[i]!=v)
						totest.push_back(v->fanin[0]->fanout[i]);
				}
			}
			else{
				cout<<"Nothing to imply7\n";
			}
		}
	}
	
	return totest;
}

 /*
 some syntax test
	Node a,b;
	vector<Gate *> null;
	a={null,8};
	b.num=2;
	a.k.push_back(&b);
	a.k.push_back(&b);
	cout<<a.num<<" "<<a.k.size()<<" "<<(a.k[0])->num<<" \n";
	*/

void assign_input(Node* v,int prefer,int times,int level){
	if(v->type==1){
		v->fanin[0]->value=times;
		v->fanin[0]->imply_level=level;
		v->imply_level=level;
		v->value=times;
	}
	else if(v->type==2){
		if(v->value==1){
			if(times==0){
				if(prefer==0){
					v->fanin[0]->value=0;
					v->fanin[0]->imply_level=level;
					v->fanin[1]->value=1;
					v->fanin[1]->imply_level=level;
				}
				else {//non prefer seen as prefer=1
					v->fanin[1]->value=0;
					v->fanin[1]->imply_level=level;
					v->fanin[0]->value=1;
					v->fanin[0]->imply_level=level;
				}
			}
			else if(times==1){
				v->fanin[0]->value=0;
				v->fanin[0]->imply_level=level;
				v->fanin[1]->value=0;
				v->fanin[1]->imply_level=level;
			}
			else{//for no prefer
				if(prefer==0||prefer==1){
					cout<<"input assignment all tried, you come here too many times\n";
					return;
				}
				v->fanin[0]->value=0;
				v->fanin[0]->imply_level=level;
				v->fanin[1]->value=1;
				v->fanin[1]->imply_level=level;
			}
		}
		else {
			if(times!=0){
				cout<<"input assignment all tried, you come here too many times\n";
				return;
			}
			v->fanin[0]->value=1;
			v->fanin[0]->imply_level=level;
			v->fanin[1]->value=1;
			v->fanin[1]->imply_level=level;
		}
	}
	else if(v->type==3){
		if(v->value==0){
			if(times==0){
				if(prefer==0){
					v->fanin[0]->value=1;
					v->fanin[0]->imply_level=level;
					v->fanin[1]->value=0;
					v->fanin[1]->imply_level=level;
				}
				else {//non prefer seen as prefer=1
					v->fanin[1]->value=1;
					v->fanin[1]->imply_level=level;
					v->fanin[0]->value=0;
					v->fanin[0]->imply_level=level;
				}
			}
			else if(times==1){
				v->fanin[0]->value=1;
				v->fanin[0]->imply_level=level;
				v->fanin[1]->value=1;
				v->fanin[1]->imply_level=level;
			}
			else{//for no prefer
				if(prefer==0||prefer==1){
					cout<<"input assignment all tried, you come here too many times\n";
					return;
				}
				v->fanin[0]->value=1;
				v->fanin[0]->imply_level=level;
				v->fanin[1]->value=0;
				v->fanin[1]->imply_level=level;
			}
		}
		else {
			if(times!=0){
				cout<<"input assignment all tried, you come here too many times\n";
				return;
			}
			v->fanin[0]->value=0;
			v->fanin[0]->imply_level=level;
			v->fanin[1]->value=0;
			v->fanin[1]->imply_level=level;
		}
	}
	else if(v->type==4){
		v->fanin[0]->value=1-v->value;
		v->fanin[0]->imply_level=level;
	}
}

int allowed_ti(Node* v, bool inpath){//v can only be on the target path//might cause problem
	if(v->type==1)
		return 2;
	if(v->type==4)//v3
		return 1;
	if(v->type==2&&v->value==0)
		return 1;
	if(v->type==2&&v->value==1)
		return 2+(1-inpath);
	if(v->type==3&&v->value==1)
		return 1;
	if(v->type==3&&v->value==0)
		return 2+(1-inpath);
	
	return 0;
}
	
void clear_value(Node* v,int level){//done
	queue<Node*> q;
	int i,size;
	q.push(v);
	while(q.size()!=0){
		cout<<q.size();
		Node* temp=q.front();
		print_node(*temp);
		q.pop();
		if(temp->imply_level==level){
			temp->imply_level=0;
			temp->value=2;
			size=temp->fanout.size();
			for(i=0;i<size;i++){
				if(temp->fanout[i]->imply_level==level)
					q.push(temp->fanout[i]);
			}
		}
		if(temp->type!=0&&temp->fanin[0]->imply_level==level)
			q.push(temp->fanin[0]);
		if(temp->type!=0&&temp->type!=4&&temp->type!=1){
			if(temp->fanin[1]->imply_level==level)
				q.push(temp->fanin[1]);
		}
		print_node(*temp);
	}
}

void simulation(vector<Node> PI){//done
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
			else if(son->type==4){//v2
				son->value=1-now->value;
				son->arrival_time=now->arrival_time+1;
				q.push(son);
			}
		}
	}
}

bool path_is_true(Path p){//不傳進來能用指標只到整張圖?//看來似乎可以//done
	int i,size=p.gates.size();
	bool true_flag=(p.gates[0]->arrival_time==0&&p.gates[size-1]->arrival_time==size-2);//v2
	
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

void print_map(vector<Node> PI,vector<Node> Gate,vector<Node> PO){//done
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

void print_node(Node v){//done
	int i,size;
	if(v.type==2)
		cout<<"NAND";
	else if(v.type==3)
		cout<<"NOR";
	else if(v.type==0)
		cout<<"PI";
	else if(v.type==1)
		cout<<"PO";
	else if(v.type==4)//v2
		cout<<"NOT";
	else 
		cout<<"UNKNOWN";
	
	cout<<"["<<v.num<<"]="<<v.value<<" @"<<v.arrival_time<<", ";
	cout<<"imply by:"<<v.imply_level<<", ";//v2
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
