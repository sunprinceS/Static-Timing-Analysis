#include "atpg.h"
bool need_to_change_input(Node* v,int level){//v belongs path, so we check need_to_change_input only when we already assign inputs,and output of it ;
//延續 path_assign，v是我們剛assign其input的gate，要check need to change or not 等同於看是否有conflict。所以我們進行implication，同時只要有衝突implication必定會改動到global variable conflict_flag
//因為input是剛assign的，所以我們把input wire接到的所有其他gate(gate input/output=此wire的gate)都加入imply序列。
//若所有能考慮的都考慮完且沒conflict，代表不用換input assignment，否則只要一conflict，就需要換input assignment
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

bool path_assign(Path p,vector<Node> PI){
//此韓式使用前最好重製所有PI,PO,gates之imply_level為0。
//t是拿來記錄path上第i個gate已經測試過幾種input可能(由allowed_ti()算最多t[i]能多大，像是NAND Gate output=0，目標是input a控制(a接角在目標path上)，那t[i]=2(a=0,b=0/1)兩種可能)
//所以邏輯就是從PO開始往前，一個gate(從PI=0數過來是第i個gate的話)若我們知道其output值，卻不知道其input值，那我們就猜它的兩個接角之input(由 assign_input() 決定assign方式)
//這兩個input接角的值是在我們考慮gate[i]的input時賦予的，所以他們(gate[i]->fanin[0/1]兩個gate)的imply_level=i
//然後我們把這個in/output都已知的gate丟到 need_to_change_input()來確認目前gate[i] input在這種assign下是否會引發邏輯上的錯誤(conflict)，若無，我們可以繼續看gate[i-1]，直到看到PI
//但是若有錯誤，我們要重新assign，而此時若t[i]>=allowed_ti()回傳的最多嘗試次數，那麼顯然是之前(更靠近PO)的gate 之input assign讓現在gate[i]在假設assignment時無論如猴都會衝突
//所以我們要改的會是前一個進行假設的gate(不一定是gate[i+1]，例如gate[i-1]是NOT Gate，你假設完gate[i-1]的值時同時imply了gate[i-2]的值，所以沿著目標path網PI走時，gate[i-1]我們會忽略
//，可以直接考慮gate[i-2]的input assign，此時若發現gate[i-2]的assign一定造成衝突，那就要回溯到gate[i]而不是gate[i-1])
//我們就在input assign與conflict所以回溯這兩個動作下，看看最後是不是能assign到PI，還是會發現PO不論是0或是1都會有conflict
//若推到PI，還是要simulation已知PI input assign(有些PI可能是未知，但simulation()還是能算出所有能得到的值)，看看目標path是否真的會是true path，若否，一樣要回溯，check是true path就回傳1代表找到了
//另一種情況是PO=0或1，仍總是有conflict，那此path就是false path 了。
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
			simulation(PI);
			if(path_is_true(p)==0){//simulation發現爛掉，回朔前一次assignment分岐
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
//times是說你第幾次近來，以免每次進來都猜一樣，並沒換assign pattern
//level是把assign的wire(output gate)標上記號，若path_assign在gate[i]的各種assign都不行要回溯時，就能用 clear_value()把imply_level=i的gate之output value設回x
//prefer是說哪個input在目標path上，input允許的話第一次assign時會把此input設成controlling value, another input設non controlling.
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

int allowed_ti(Node* v, bool inpath){
//inpath是說是否在target path上，但目前似乎呼叫此函數時一定在path上，可能是多餘的
	if(v->type==1)
		return 2;
	if(v->type==4)//v3
		return 1;
	if(v->type==2&&v->value==0)//1NAND1=0 
		return 1;
	if(v->type==2&&v->value==1)//inpath代表有prefer，假設prefer a rather than b，aNANDb就不會給1NAND0，因為此時a就不會造成truepath了。所以只剩0NAND0,0NAND1可以造出output1，即t最多2次，沒prefer的話1NAND0可接受，所以多1
		return 2+(1-inpath);
	if(v->type==3&&v->value==1)
		return 1;
	if(v->type==3&&v->value==0)
		return 2+(1-inpath);
	
	return 0;
}

vector<Node*> implication(Node* v,int level){
//給你一個gate，你判斷有沒有更多x(.value=2)者能得出值
//當我們得到一個wire的值時，用到此wire的gate都要去檢查有沒有更多x的wire便已知，即看看有沒有implication
//要檢查的gate會回傳回 need_to_change_input
	vector<Node*> totest;
	//PI/PO dont need implication
	cout<<"implying node with level="<<level<<":\n";
	print_node(*v);
	
	if(v->type==0||v->type==1){
		if(v->type==1){
			v->value=v->fanin[0]->value;
			v->imply_level=v->fanin[0]->imply_level;
		}
		cout<<"No imply for PI/PO\n";
		return totest;
	}
	//check conflict 是檢查是否有邏輯錯誤，即in/output value不符合gate功能
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
			if(v->fanin[0]->value==0&&v->fanin[1]->value!=0){//aNANDb=y,y=x,a=0,b=0  => y=1
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