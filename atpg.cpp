#include "atpg.h"
bool need_to_change_input(Node* v,int level){//level=p[i] in path
	queue<Node*> q;
	int i,size,l=level,f0size=v->fanin[0]->fanout.size(),f1size=v->fanin[1]->fanout.size();
	q.push(v->fanin[0]);
	q.push(v->fanin[1]);
	for(i=0;i<f0size;i++){
		q.push(v->fanin[0]->fanout[i]);
	}
	for(i=0;i<f1size;i++){
		q.push(v->fanin[1]->fanout[i]);
	}
	conflict_flag=0;
	while(q.size()!=0){
		vector<Node*> newq=implication(q.front(),l);

		if(conflict_flag==1)
			return 1;
		q.pop();
		size=newq.size();
		for(i=0;i<size;i++){
			q.push(newq[i]);
		}
	}
	return 0;
}

void path_assign(Path p){//need to clear imply_level of all gates
	int i,size=p.gates.size();
	vector<int> t;
	t.resize(size,0);
	
	for(i=size-1;i>0;i--){
		if(p.gates[i]->value!=2){
			continue;
		}
		if(t[i]>=allowed_ti(p.gates[i])){//NAND NOR gate only 3 input allowed,PO only 2 allowed
			t[i]=0;
			i+=2;
		}
		if(p.gates[i]->fanin[0]==p.gates[i-1])
			assign_input(p.gates[i],0,t[i],i);
		if(p.gates[i]->fanin[1]==p.gates[i-1])
			assign_input(p.gates[i],1,t[i],i);
		t[i]++;
		if(need_to_change_input(p.gates[i],size-i)){
			clear_value(p.gates[i],size-i);
			i+=1;
		}
	}
}
void assign_input(Node* v,int prefer,int times,int level){
	if(v->type==1){
		v->fanin[0]->value=times;
		v->fanin[0]->imply_level=level;
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
				v->fanin[0]->value=0;
				v->fanin[0]->imply_level=level;
				v->fanin[1]->value=1;
				v->fanin[1]->imply_level=level;
			}
		}
		else {
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
				v->fanin[0]->value=1;
				v->fanin[0]->imply_level=level;
				v->fanin[1]->value=0;
				v->fanin[1]->imply_level=level;
			}
		}
		else {
			v->fanin[0]->value=0;
			v->fanin[0]->imply_level=level;
			v->fanin[1]->value=0;
			v->fanin[1]->imply_level=level;
		}
	}
}
void clear_value(Node* v,int level){
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
			//v2
			if(temp->type!=0&&temp->fanin[0]->imply_level==level)
				q.push(temp->fanin[0]);
			if(temp->type!=0&&temp->type!=4&&temp->type!=1&&temp->fanin[1]->imply_level==level)
				q.push(temp->fanin[1]);
			size=temp->fanout.size();
			for(i=0;i<size;i++){
				if(temp->fanout[i]->imply_level==level)
					q.push(temp->fanout[i]);
			}
		}
		print_node(*temp);
	}
}
int allowed_ti(Node* v){
	if(v->type==1)
		return 2;
	if(v->type==2&&v->value==0)
		return 1;
	if(v->type==2&&v->value==1)
		return 2;
	if(v->type==3&&v->value==1)
		return 1;
	if(v->type==3&&v->value==0)
		return 2;
	return 0;
}

vector<Node*> implication(Node* v,int level){
	vector<Node*> totest;
	//PI/PO dont need to implication
	if(v->type==0||v->type==1)
		return totest;
	//check conflict
	if((v->fanin[1]->value==0||v->fanin[1]->value==0)&&v->value==0&&v->type==2){
		//conflict_flag=1;
		//return totest;
	//}
	//if((v->fanin[1]->value==1&&v->fanin[1]->value==1)&&v->value==1&&v->type==2){
		//conflict_flag=1;
		//return totest;
	//}
	//if((v->fanin[1]->value==0&&v->fanin[1]->value==0)&&v->value==0&&v->type==3){
		//conflict_flag=1;
		//return totest;
	//}
	//if((v->fanin[1]->value==1||v->fanin[1]->value==1)&&v->value==1&&v->type==3){
		//conflict_flag=1;
		//return totest;
	//}
	
	////if find some implication, check if the new known value wire will cause other implication
	//int i,size;
	//if(v->type==2){//NAND
		//if(v->value==2){
			//if(v->fanin[0]->value==0&&v->fanin[1]->value!=0){
				//v->value=1;
				//v->imply_level=level;
				////v->arrival_time=v->fanin[0]->arrival_time;
				//size=v->fanout.size();
				//for(i=0;i<size;i++)
					//totest.push_back(v->fanout[i]);
			//}
			//else if(v->fanin[1]->value==0&&v->fanin[0]->value!=0){
				//v->value=1;
				//v->imply_level=level;
				////v->arrival_time=v->fanin[1]->arrival_time;
				//size=v->fanout.size();
				//for(i=0;i<size;i++)
					//totest.push_back(v->fanout[i]);
			//}
			//else if(v->fanin[1]->value==0&&v->fanin[0]->value==0){
				//v->value=1;
				//v->imply_level=level;
				////v->arrival_time=min(v->fanin[1]->arrival_time, v->fanin[0]->arrival_time);
				//size=v->fanout.size();
				//for(i=0;i<size;i++)
					//totest.push_back(v->fanout[i]);
			//}
			//else if(v->fanin[1]->value==1&&v->fanin[0]->value==1){
				//v->value=0;
				//v->imply_level=level;
				////v->arrival_time=max(v->fanin[1]->arrival_time, v->fanin[0]->arrival_time);	
				//size=v->fanout.size();
				//for(i=0;i<size;i++)
					//totest.push_back(v->fanout[i]);
			//}
		//}
		//else if(v->value==0){
			//if(v->fanin[0]->value==2){
				//v->fanin[0]->value=1;
				//v->fanin[0]->imply_level=level;
				//totest.push_back(v->fanin[0]);
				//size=v->fanin[0]->fanout.size();
				//for(i=0;i<size;i++)
					//totest.push_back(v->fanin[0]->fanout[i]);
			//}
			//if(v->fanin[1]->value==2){
				//v->fanin[1]->value=1;
				//v->fanin[1]->imply_level=level;
				//totest.push_back(v->fanin[1]);
				//size=v->fanin[1]->fanout.size();
				//for(i=0;i<size;i++)
					//totest.push_back(v->fanin[1]->fanout[i]);
			//}
		//}
		//else if(v->value==1){
			//if(v->fanin[0]->value==1&&v->fanin[1]->value==2){
				//v->fanin[1]->value=0;
				//v->fanin[1]->imply_level=level;
				//totest.push_back(v->fanin[1]);
				//size=v->fanin[1]->fanout.size();
				//for(i=0;i<size;i++)
					//totest.push_back(v->fanin[1]->fanout[i]);
			//}
			//else if(v->fanin[0]->value==2&&v->fanin[1]->value==1){
				//v->fanin[0]->value=0;
				//v->fanin[0]->imply_level=level;
				//totest.push_back(v->fanin[0]);
				//size=v->fanin[0]->fanout.size();
				//for(i=0;i<size;i++)
					//totest.push_back(v->fanin[0]->fanout[i]);
			//}
		//}
	//}
	//else if(v->type==3){//NOR
		//if(v->value==2){
			//if(v->fanin[0]->value==1&&v->fanin[1]->value!=1){
				//v->value=0;
				//v->imply_level=level;
				////v->arrival_time=v->fanin[0]->arrival_time;
				//size=v->fanout.size();
				//for(i=0;i<size;i++)
					//totest.push_back(v->fanout[i]);
			//}
			//else if(v->fanin[1]->value==1&&v->fanin[0]->value!=1){
				//v->value=0;
				//v->imply_level=level;
				////v->arrival_time=v->fanin[1]->arrival_time;
				//size=v->fanout.size();
				//for(i=0;i<size;i++)
					//totest.push_back(v->fanout[i]);
			//}
			//else if(v->fanin[1]->value==1&&v->fanin[0]->value==1){
				//v->value=0;
				//v->imply_level=level;
				////v->arrival_time=min(v->fanin[1]->arrival_time, v->fanin[0]->arrival_time);
				//size=v->fanout.size();
				//for(i=0;i<size;i++)
					//totest.push_back(v->fanout[i]);
			//}
			//else if(v->fanin[1]->value==0&&v->fanin[0]->value==0){
				//v->value=1;
				//v->imply_level=level;
				////v->arrival_time=max(v->fanin[1]->arrival_time, v->fanin[0]->arrival_time);	
				//size=v->fanout.size();
				//for(i=0;i<size;i++)
					//totest.push_back(v->fanout[i]);
			//}
		//}
		//else if(v->value==1){
			//if(v->fanin[0]->value==2){
				//v->fanin[0]->value=0;
				//v->fanin[0]->imply_level=level;
				//totest.push_back(v->fanin[0]);
				//size=v->fanin[0]->fanout.size();
				//for(i=0;i<size;i++)
					//totest.push_back(v->fanin[0]->fanout[i]);
			//}
			//if(v->fanin[1]->value==2){
				//v->fanin[1]->value=0;
				//v->fanin[1]->imply_level=level;
				//totest.push_back(v->fanin[1]);
				//size=v->fanin[1]->fanout.size();
				//for(i=0;i<size;i++)
					//totest.push_back(v->fanin[1]->fanout[i]);
			//}
		//}
		//else if(v->value==0){
			//if(v->fanin[0]->value==0&&v->fanin[1]->value==2){
				//v->fanin[1]->value=1;
				//v->fanin[1]->imply_level=level;
				//totest.push_back(v->fanin[1]);
				//size=v->fanin[1]->fanout.size();
				//for(i=0;i<size;i++)
					//totest.push_back(v->fanin[1]->fanout[i]);
			//}
			//else if(v->fanin[0]->value==2&&v->fanin[1]->value==0){
				//v->fanin[0]->value=1;
				//v->fanin[0]->imply_level=level;
				//totest.push_back(v->fanin[0]);
				//size=v->fanin[0]->fanout.size();
				//for(i=0;i<size;i++)
					//totest.push_back(v->fanin[0]->fanout[i]);
			//}
		//}
	//}
	
	//return totest;
//}

