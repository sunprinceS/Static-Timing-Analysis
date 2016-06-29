#include "atpg.h"
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
