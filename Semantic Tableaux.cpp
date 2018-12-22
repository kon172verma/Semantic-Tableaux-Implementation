
//*****************************************************************************************************************//

// 	Name 			: 	Konark Verma	//
// 	Roll number 	: 	2018MCS2025		//
// 	COL-765 		: 	Assignment 2	//
// 	Question-1							//

//*****************************************************************************************************************//

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <stack>

using namespace std;

//*****************************************************************************************************************//

string clause;
string String;

char buffer[10000];
char tokencounter[2] = {'A','A'};

struct datanode{
	char token[2];
	string expr;
};
vector<datanode*> map;

struct exprnode{
	string str;
};

struct ltrlnode{
	char ltrl[2];
};

struct node{
	vector<exprnode> expr;
	vector<ltrlnode> literals;
	string comment;
	int close;
	node* left;
	node* right;
	node* center;
	node* parent;
};
node* ROOT, *ROOT2;

int pathscount;
vector<node*> openpaths;
vector<node*> closepaths;

//*****************************************************************************************************************//

void incrementtokencounter();

void readclause();
void parseclause();

void parsesubclause();
void parsenegatation();
void parsebinaryoperator(char);
void insertnode(int,int);
void getexpression();
void printmap();
void printexpression();

void createtree();
void extendtree(node*);
void findchildren(node*);
void printpaths();
int printresult();

//*****************************************************************************************************************//
//*****************************************************************************************************************//
//*****************************************************************************************************************//

int main(){

	readclause();
	parseclause();

    createtree();
	findchildren(ROOT);
	printpaths();
	int result = printresult();

	if(result){

		pathscount=0;
		openpaths.clear();
		closepaths.clear();

		char ch1,ch0;
		if(tokencounter[1]=='A'){
			ch0 = tokencounter[0]-1; ch1 = 'Z';
		}else{
			ch0 = tokencounter[0]; ch1 = tokencounter[1]-1;
		}

		sprintf(buffer,"-%c%c",ch0,ch1);
		string temp(buffer);

		datanode *node = new datanode;
		node->token[0] = tokencounter[0];
		node->token[1] = tokencounter[1];
		node->expr = temp;
		map.push_back(node);

		ROOT2 = ROOT;
		createtree();
		findchildren(ROOT);

		if(openpaths.size()==0 && closepaths.size()!=0)
			cout<<endl<<"Entered clause is Valid."<<endl;
		
	}

	return 0;
}

//*****************************************************************************************************************//
//*****************************************************************************************************************//
//*****************************************************************************************************************//

void printmap(){
	for(int i=0; i<map.size(); i++){
		cout<<map[i]->token[0]<<map[i]->token[1]<<" "<<map[i]->expr<<endl;
	}
}

//*****************************************************************************************************************//

void printexpression(){
	sprintf(buffer,"%c%c",map[map.size()-1]->token[0],map[map.size()-1]->token[1]);
	string str(buffer);
	String = str;
	getexpression();
	cout<<String<<endl;
}

//*****************************************************************************************************************//

void incrementtokencounter(){
	if(tokencounter[1]=='Z'){
		tokencounter[0]++;
		tokencounter[1]='A';
	}else
		tokencounter[1]++;
}

//*****************************************************************************************************************//

void readclause(){
	getline(cin, clause);
}

//*****************************************************************************************************************//

void parseclause(){

	stack<int> Stack;
	int begin, end;
	string original = clause;
	int n = clause.size();
	char ch0, ch1;

	for(int i=0; i < n; i++){
		if(original[i]=='(')
			Stack.push(i);
		else if(original[i]==')'){
			begin = Stack.top();
			Stack.pop();
			end = i;
			clause = original.substr(begin+1,end-begin-1);
			parsesubclause();
			if(tokencounter[1]=='A'){
				ch0 = tokencounter[0]-1; ch1 = 'Z';
			}else{
				ch0 = tokencounter[0]; ch1 = tokencounter[1]-1;
			}
			original[begin] = ch0;
			original[begin+1] = ch1;

			n-=(end-begin-1);
			for(int k=begin+2, j=1; k <= n; j++,k++)
				original[k] = original[end+j];

			i=begin+1;
		}
		else{}
	}

	for(int k=n; k < original.size(); k++)
		original[k] = '\0';
	clause = original;

	parsesubclause();

}

//*****************************************************************************************************************//

void parsesubclause(){
	if(clause.size()==1){
		insertnode(0,0);
		clause.push_back('e');
		clause[0] = tokencounter[0];
		clause[1] = tokencounter[1];
		incrementtokencounter();
		return;
	}
	parsenegatation();
	parsebinaryoperator('.');
	parsebinaryoperator('+');
	parsebinaryoperator('>');
	parsebinaryoperator('=');
}

//*****************************************************************************************************************//

void parsenegatation(){
	int n = clause.size();
	for(int i=0; i < n; i++){
		if(clause[i]=='-'){
			if(clause[i+1]=='-'){
				int j=i+1;
				while(clause[j]=='-')j++;
				int start=i;
				j--;
				while(j>=start){
					if(clause[j+1]>='a' && clause[j+1]<='z'){
						insertnode(j,j+1);
						clause[j] = tokencounter[0];
						clause[j+1] = tokencounter[1];
						incrementtokencounter();
						j--;
					}
					else if(clause[j+1]>='A' && clause[j+1]<='Z'){
						insertnode(j,j+2);
						clause[j] = tokencounter[0];
						clause[j+1] = tokencounter[1];
						n--;
						for(int k=j+2; k <= n; k++)
							clause[k] = clause[k+1];
						incrementtokencounter();
						j--;
					}
				}
			}
			else if(clause[i+1]>='a' && clause[i+1]<='z'){
				insertnode(i,i+1);
				clause[i] = tokencounter[0];
				clause[i+1] = tokencounter[1];
				incrementtokencounter();
			}
			else if(clause[i+1]>='A' && clause[i+1]<='Z'){
				insertnode(i,i+2);
				clause[i] = tokencounter[0];
				clause[i+1] = tokencounter[1];
				n--;
				for(int k=i+2; k <= n; k++)
					clause[k] = clause[k+1];
				incrementtokencounter();
			}
		}
	}
}

//*****************************************************************************************************************//

void parsebinaryoperator(char optr){
	int n = clause.size();
	int k;
	for(int i=0; i < n; i++){
		if(clause[i]==optr){
			if(clause[i-1]>='a' && clause[i-1]<='z'){
				if(clause[i+1]>='a' && clause[i+1]<='z'){
					insertnode(i-1,i+1);
					clause[i-1] = tokencounter[0];
					clause[i] = tokencounter[1];
					n--;
					for(k=i+1; k < n; k++)
						clause[k] = clause[k+1];
					clause[k] = '\0';
					incrementtokencounter();
				}
				else if(clause[i+1]>='A' && clause[i+1]<='Z'){
					insertnode(i-1,i+2);
					clause[i-1] = tokencounter[0];
					clause[i] = tokencounter[1];
					n-=2;
					for(k=i+1; k < n; k++)
						clause[k] = clause[k+2];
					clause[k] = '\0';
					incrementtokencounter();
				}
			}
			else if(clause[i-1]>='A' && clause[i-1]<='Z'){
				if(clause[i+1]>='a' && clause[i+1]<='z'){
					insertnode(i-2,i+1);
					clause[i-2] = tokencounter[0];
					clause[i-1] = tokencounter[1];
					n-=2;
					for(k=i; k < n; k++)
						clause[k] = clause[k+2];
					clause[k] = '\0';
					incrementtokencounter();
					i--;
				}
				else if(clause[i+1]>='A' && clause[i+1]<='Z'){
					insertnode(i-2,i+2);
					clause[i-2] = tokencounter[0];
					clause[i-1] = tokencounter[1];
					n-=3;
					for(k=i; k < n; k++)
						clause[k] = clause[k+3];
					clause[k] = '\0';
					incrementtokencounter();
					i--;
				}
			}
		}
	}
	for(k=n; k < clause.size(); k++)
		clause[k] = '\0';
}

//*****************************************************************************************************************//

void insertnode(int start, int end){

	datanode *node = new datanode;
	node->token[0] = tokencounter[0];
	node->token[1] = tokencounter[1];

	sprintf(buffer,"%c",clause[start]);
	for(int i=start+1;i<=end;i++)
		sprintf(buffer,"%s%c",buffer,clause[i]);

	string str(buffer);
	node->expr = str;
	map.push_back(node);
}

//*****************************************************************************************************************//

void getexpression(){

	char ch0,ch1;
	int index, size;
	int n;
	n = String.size();

	for(int i=0;i<n;i++){

		if(String[i]>='A' && String[i]<='Z'){
			ch0 = String[i];
			ch1 = String[i+1];
			index = 26*(ch0-'A') + ch1-'A';
			size = (map[index]->expr).size();

			if(size==1){
				n--;
				for(int j=i+1; j<n; j++)
					String[j] = String[j+1];
				String = String.substr(0, String.size()-1);
				String[i] = (map[index]->expr)[0];
			}
			else{
				n+=size;
				for(int j=0;j<size;j++)
					String.push_back('e');
				for(int j=n-size-1; j>i+1; j--)
					String[j+size] = String[j];
				String[i] = '(';
				String[i+size+1] = ')';
				for(int j=0; j<size; j++)
					String[i+j+1] = (map[index]->expr)[j];
			}
		}
	}
}

//*****************************************************************************************************************//

void createtree(){

	exprnode* expression = new exprnode;
	expression->str = map[map.size()-1]->expr;

	ROOT = new node;
	(ROOT->expr).push_back(*expression);

	ROOT->literals.clear();
	ROOT->comment = "Path open.";

	ROOT->close = 0;
	ROOT->left = NULL;
	ROOT->right = NULL;
	ROOT->center = NULL;
	ROOT->parent = NULL;

    extendtree(ROOT);
}

//******************************************************************************************************************//

void extendtree(node* root){

	int n, size;

	n = (root->expr).size();
	vector<int> type;

	//Classification of the expressions.
	for(int i=0; i<n; i++){

		size = ((((root->expr)[i]).str)).size();

		if(size == 1){
			type.push_back(1);
			continue;
		}
		else if(size == 2 && (((root->expr)[i]).str)[0] == '-'){
			type.push_back(2);
			continue;
		}
		else if(size == 3 && (((root->expr)[i]).str)[0] == '-'){
			type.push_back(3);
		}
		else{

			for(int j=0;j<size;j++){
				if((((root->expr)[i]).str)[j]=='.'){
					type.push_back(11);
					break;
				}
				else if((((root->expr)[i]).str)[j]=='+'){
					type.push_back(12);
					break;
				}
				else if((((root->expr)[i]).str)[j]=='>'){
					type.push_back(13);
					break;
				}
				else if((((root->expr)[i]).str)[j]=='='){
					type.push_back(14);
					break;
				}
				else
					continue;
			}
		}

		//further classification of negatation expressions.
		if(type[i]==3){

			char ch0 = (((root->expr)[i]).str)[1];
			char ch1 = (((root->expr)[i]).str)[2];
			int index = 26*(ch0-'A')+ch1-'A';
			string temp = map[index]->expr;
			int size = temp.size();

			if(size == 1)
				type[i] = 18;
			else if(size == 2 && temp[0] == '-')
				type[i] = 19;
			else if(size == 3 && temp[0] == '-')
				type[i] = 20;
			else{
				for(int j=0;j<size;j++){

					if(temp[j]=='.'){
						type[i] = 21;
						break;
					}
					else if(temp[j]=='+'){
						type[i] = 22;
						break;
					}
					else if(temp[j]=='>'){
						type[i] = 23;
						break;
					}
					else if(temp[j]=='='){
						type[i] = 24;
						break;
					}
					else
						continue;
				}
			}
		}
	}

	//****************************** Creating a new temp node and resolving the literals *****************************//

	//creating a new temp node.
    node temp1 = *root;

    //n is the number of expressions.
    int temp_n = n;

    //converting the expressions into literals.
	for(int i=0; i<temp_n; i++){

		//if my expression is a literal.
        if(type[i]==1){
            ltrlnode *ltrl0 = new ltrlnode;
            ltrl0->ltrl[0] = '+';
            ltrl0->ltrl[1] = (((temp1.expr)[i]).str)[0];
            
            int flag=0, n1=(temp1.literals).size();
            for(int j=0; j<n1; j++){
            	if( ((temp1.literals)[j]).ltrl[0] == (*ltrl0).ltrl[0] && ((temp1.literals)[j]).ltrl[1] == (*ltrl0).ltrl[1] ){
            		flag=1;
            		break;
            	}
            }
            if(!flag) 
            	(temp1.literals).push_back(*ltrl0);

            (temp1.expr).erase((temp1.expr).begin() + i);
            type.erase(type.begin()+i);
            i--;
            temp_n--;
        }

        //if my expression is negation of a literal.
        else if(type[i]==2){
            ltrlnode *ltrl0 = new ltrlnode;
            ltrl0->ltrl[0] = '-';
            ltrl0->ltrl[1] = (((temp1.expr)[i]).str)[1];
            
            int flag=0, n1=temp1.literals.size();
            for(int j=0; j<n1; j++){
            	if( ((temp1.literals)[j]).ltrl[0] == (*ltrl0).ltrl[0] && ((temp1.literals)[j]).ltrl[1] == (*ltrl0).ltrl[1] ){
            		flag=1;
            		break;
            	}
            }
            if(!flag) 
            	(temp1.literals).push_back(*ltrl0);

            (temp1.expr).erase((temp1.expr).begin() + i);
            type.erase(type.begin()+i);
            i--;
            temp_n--;
        }

        //if my expression is negation and contains a literal.
        else if(type[i]==18){
            char ch0 = (((temp1.expr)[i]).str)[1];
			char ch1 = (((temp1.expr)[i]).str)[2];
			int index = 26*(ch0-'A')+ch1-'A';
			string tmp = map[index]->expr;

            ltrlnode *ltrl0 = new ltrlnode;
            ltrl0->ltrl[0] = '-';
            ltrl0->ltrl[1] = tmp[0];

            int flag=0, n1=temp1.literals.size();
            for(int j=0; j<n1; j++){
            	if( ((temp1.literals)[j]).ltrl[0] == (*ltrl0).ltrl[0] && ((temp1.literals)[j]).ltrl[1] == (*ltrl0).ltrl[1] ){
            		flag=1;
            		break;
            	}
            }
            if(!flag) 
            	(temp1.literals).push_back(*ltrl0);

            (temp1.expr).erase((temp1.expr).begin() + i);
            type.erase(type.begin()+i);
            i--;
            temp_n--;
        }
	}

	int value = (temp1.literals).size();

	//Checking for the contradicting literals.
	for(int i=0; i<value-1; i++){
		for(int j=i+1; j<value; j++){

			if(		(((temp1.literals)[i]).ltrl[0]) != (((temp1.literals)[j]).ltrl[0]) 
				&& (((temp1.literals)[i]).ltrl[1]) == (((temp1.literals)[j]).ltrl[1]) )
			{
				temp1.close = 1;
				sprintf(buffer,"Path closed because contradicting literals found, which are : %c and -%c"
									,(((temp1.literals)[i]).ltrl[1]),(((temp1.literals)[i]).ltrl[1]) );
				temp1.comment = buffer;
				break;
			}
		}
		if(temp1.close == 1)
			break;
	}

	//if path has to be closed, close it and return.
	if(temp1.close==1){
		root->close = 1;
		root->comment = temp1.comment;
		root->left = NULL;
		root->right = NULL;
		root->center = NULL;
		return;
	}

	int flag = 0;
	int i;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//checking for negation negation single literal case.
	for(i=0; i<type.size(); i++){
		if(type[i]==19){
			flag = 1;
			break;
		}
	}
	if(flag==1){
        char ch0 = (((temp1.expr)[i]).str)[1];
		char ch1 = (((temp1.expr)[i]).str)[2];
		int index = 26*(ch0-'A')+ch1-'A';
		string tmp = map[index]->expr;
		tmp = tmp.substr(1,1);

		(((temp1.expr)[i]).str) = tmp;

		node *centernode = new node;
		*centernode = temp1;

		centernode->left = NULL;
	    centernode->right = NULL;
		centernode->center = NULL;
		centernode->parent = root;
		extendtree(centernode);

		sprintf(buffer,"Rule 5 applied on : --");
		string buffer1(buffer);
		buffer1.append(tmp);
		root->comment = temp1.comment = buffer1;
		root->center = centernode;
		root->left = NULL;
		root->right = NULL;

		return;
    }


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//checking for negation negation case.
	for(i=0; i<type.size(); i++){
		if(type[i]==20){
			flag = 1;
			break;
		}
	}
	if(flag==1){
        char ch0 = (((temp1.expr)[i]).str)[1];
		char ch1 = (((temp1.expr)[i]).str)[2];
		int index = 26*(ch0-'A')+ch1-'A';
		string tmp = map[index]->expr;
		tmp = tmp.substr(1,2);

		String = (((temp1.expr)[i]).str).substr(1,2);
		getexpression();

		if(tmp.size()==2 && tmp[0]>='A' && tmp[0]<='Z'){
			char ch0 = tmp[0];
			char ch1 = tmp[1];
			int index = 26*(ch0-'A')+ch1-'A';
			tmp = map[index]->expr;
		}

		(((temp1.expr)[i]).str) = tmp;

		node *centernode = new node;
		*centernode = temp1;

		centernode->left = NULL;
	    centernode->right = NULL;
		centernode->center = NULL;
		centernode->parent = root;
		extendtree(centernode);

		sprintf(buffer,"Rule 5 applied on : -");
		string buffer1(buffer);
		buffer1.append(String);
		root->comment = temp1.comment = buffer1;
		root->center = centernode;
		root->left = NULL;
		root->right = NULL;

		return;
    }

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //checking for 'and' case.
	for(i=0; i<type.size(); i++){
		if(type[i]==11){
			flag = 1;
			break;
		}
	}
	if(flag==1){

		string tmp = (((temp1.expr)[i]).str);
		string output = tmp;
		int j;
		for(j=0; j<tmp.size(); j++)
			if(tmp[j]=='.')
				break;

		string tmp1 = tmp.substr(0,j);
		if(tmp1.size()==2 && tmp1[0]>='A' && tmp1[0]<='Z'){
			char ch0 = tmp1[0];
			char ch1 = tmp1[1];
			int index = 26*(ch0-'A')+ch1-'A';
			tmp1 = map[index]->expr;
		}

		string tmp2 = tmp.substr(j+1,tmp.size()-j);
		if(tmp2.size()==2 && tmp2[0]>='A' && tmp2[0]<='Z'){
			char ch0 = tmp2[0];
			char ch1 = tmp2[1];
			int index = 26*(ch0-'A')+ch1-'A';
			tmp2 = map[index]->expr;
		}

		((temp1.expr)[i]).str = tmp1;
		exprnode* expression = new exprnode;
		expression->str = tmp2;
		(temp1.expr).push_back(*expression);

		node *centernode = new node;
		*centernode = temp1;

		centernode->left = NULL;
	    centernode->right = NULL;
		centernode->center = NULL;
		centernode->parent = root;
		extendtree(centernode);

		string finalcomment = "Rule 1 applied on : ";
		String = output;
		getexpression();
		finalcomment.append(String);
		finalcomment.append("\n\tResultant elements : ");
		String = tmp1;
		getexpression();
		finalcomment.append(String);
		finalcomment.append(" , ");
		String = tmp2;
		getexpression();
		finalcomment.append(String);

		root->comment = temp1.comment = finalcomment;		
		root->center = centernode;
		root->left = NULL;
		root->right = NULL;

		return;
    }

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//checking for 'negation or' case.
	for(i=0; i<type.size(); i++){
		if(type[i]==22){
			flag = 1;
			break;
		}
	}
	if(flag==1){

		char ch0 = (((temp1.expr)[i]).str)[1];
		char ch1 = (((temp1.expr)[i]).str)[2];
		int index = 26*(ch0-'A')+ch1-'A';
		string tmp = map[index]->expr;
		string output = ((temp1.expr)[i]).str;

		int j;
		for(j=0; j<tmp.size(); j++)
			if(tmp[j]=='+')
				break;

		string tmp0;

		tmp0 = "-";
		string tmp1 = tmp.substr(0,j);
		tmp1 = tmp0.append(tmp1);

		tmp0 = "-";
		string tmp2 = tmp.substr(j+1,tmp.size()-j);
		tmp2 = tmp0.append(tmp2);

		((temp1.expr)[i]).str = tmp1;
		exprnode* expression = new exprnode;
		expression->str = tmp2;
		(temp1.expr).push_back(*expression);

		node *centernode = new node;
		*centernode = temp1;

		centernode->left = NULL;
	    centernode->right = NULL;
		centernode->center = NULL;
		centernode->parent = root;
		extendtree(centernode);

		string finalcomment = "Rule 4 applied on : ";
		String = output;
		getexpression();
		finalcomment.append(String);
		finalcomment.append("\n\tResultant elements : ");
		String = tmp1;
		getexpression();
		finalcomment.append(String);
		finalcomment.append(" , ");
		String = tmp2;
		getexpression();
		finalcomment.append(String);

		root->comment = temp1.comment = finalcomment;
		root->center = centernode;
		root->left = NULL;
		root->right = NULL;

		return;
    }

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//checking for 'negation implies' case.
	for(i=0; i<type.size(); i++){
		if(type[i]==23){
			flag = 1;
			break;
		}
	}
	if(flag==1){

		char ch0 = (((temp1.expr)[i]).str)[1];
		char ch1 = (((temp1.expr)[i]).str)[2];
		int index = 26*(ch0-'A')+ch1-'A';
		string tmp = map[index]->expr;
		string output = ((temp1.expr)[i]).str;

		int j;
		for(j=0; j<tmp.size(); j++)
			if(tmp[j]=='>')
				break;

		string tmp1 = tmp.substr(0,j);
		if(tmp1.size()==2 && tmp1[0]>='A' && tmp1[0]<='Z'){
			char ch0 = tmp1[0];
			char ch1 = tmp1[1];
			int index = 26*(ch0-'A')+ch1-'A';
			tmp1 = map[index]->expr;
		}

		string tmp0 = "-";
		string tmp2 = tmp.substr(j+1,tmp.size()-j);
		tmp2 = tmp0.append(tmp2);

		((temp1.expr)[i]).str = tmp1;
		exprnode* expression = new exprnode;
		expression->str = tmp2;
		(temp1.expr).push_back(*expression);

		node *centernode = new node;
		*centernode = temp1;

		centernode->left = NULL;
	    centernode->right = NULL;
		centernode->center = NULL;
		centernode->parent = root;
		extendtree(centernode);

		string finalcomment = "Rule 7 applied on : ";
		String = output;
		getexpression();
		finalcomment.append(String);
		finalcomment.append("\n\tResultant elements : ");
		String = tmp1;
		getexpression();
		finalcomment.append(String);
		finalcomment.append(" , ");
		String = tmp2;
		getexpression();
		finalcomment.append(String);

		root->comment = temp1.comment = finalcomment;
		root->center = centernode;
		root->left = NULL;
		root->right = NULL;
		return;
    }

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//checking for 'or' case.
	for(i=0; i<type.size(); i++){
		if(type[i]==12){
			flag = 1;
			break;
		}
	}
	if(flag==1){

		string tmp = ((temp1.expr)[i]).str;
		string output = tmp;

		int j;
		for(j=0; j<tmp.size(); j++)
			if(tmp[j]=='+')
				break;

		string tmp1 = tmp.substr(0,j);
		if(tmp1.size()==2 && tmp1[0]>='A' && tmp1[0]<='Z'){
			char ch0 = tmp1[0];
			char ch1 = tmp1[1];
			int index = 26*(ch0-'A')+ch1-'A';
			tmp1 = map[index]->expr;
		}

		string tmp2 = tmp.substr(j+1,tmp.size()-j);
		if(tmp2.size()==2 && tmp2[0]>='A' && tmp2[0]<='Z'){
			char ch0 = tmp2[0];
			char ch1 = tmp2[1];
			int index = 26*(ch0-'A')+ch1-'A';
			tmp2 = map[index]->expr;
		}

		((temp1.expr)[i]).str = tmp1;
		node *leftnode = new node;
		*leftnode = temp1;
		leftnode->left = NULL;
	    leftnode->right = NULL;
		leftnode->center = NULL;
		leftnode->parent = root;
		extendtree(leftnode);

		((temp1.expr)[i]).str = tmp2;
		node *rightnode = new node;
		*rightnode = temp1;
		rightnode->left = NULL;
	    rightnode->right = NULL;
		rightnode->center = NULL;
		rightnode->parent = root;
		extendtree(rightnode);

		string finalcomment = "Rule 3 applied on : ";
		String = output;
		getexpression();
		finalcomment.append(String);
		finalcomment.append("\n\tResultant elements : ");
		String = tmp1;
		getexpression();
		finalcomment.append(String);
		finalcomment.append(" , ");
		String = tmp2;
		getexpression();
		finalcomment.append(String);

		root->comment = temp1.comment = finalcomment;
		root->center = NULL;
		root->left = leftnode;
		root->right = rightnode;

		return;
    }


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//checking for 'implies' case.
	for(i=0; i<type.size(); i++){
		if(type[i]==13){
			flag = 1;
			break;
		}
	}
	if(flag==1){

		string tmp = ((temp1.expr)[i]).str;
		string output = tmp;

		int j;
		for(j=0; j<tmp.size(); j++)
			if(tmp[j]=='>')
				break;

		string tmp0 = "-";
		string tmp1 = tmp.substr(0,j);
		tmp1 = tmp0.append(tmp1);
		((temp1.expr)[i]).str = tmp1;

		string tmp2 = tmp.substr(j+1,tmp.size()-j);
		if(tmp2.size()==2 && tmp2[0]>='A' && tmp2[0]<='Z'){
			char ch0 = tmp2[0];
			char ch1 = tmp2[1];
			int index = 26*(ch0-'A')+ch1-'A';
			tmp2 = map[index]->expr;
		}

		node *leftnode = new node;
		*leftnode = temp1;
		leftnode->left = NULL;
	    leftnode->right = NULL;
		leftnode->center = NULL;
		leftnode->parent = root;
		extendtree(leftnode);

		((temp1.expr)[i]).str = tmp2;
		node *rightnode = new node;
		*rightnode = temp1;
		rightnode->left = NULL;
	    rightnode->right = NULL;
		rightnode->center = NULL;
		rightnode->parent = root;
		extendtree(rightnode);

		string finalcomment = "Rule 6 applied on : ";
		String = output;
		getexpression();
		finalcomment.append(String);
		finalcomment.append("\n\tResultant elements : ");
		String = tmp1;
		getexpression();
		finalcomment.append(String);
		finalcomment.append(" , ");
		String = tmp2;
		getexpression();
		finalcomment.append(String);

		root->comment = temp1.comment = finalcomment;
		root->center = NULL;
		root->left = leftnode;
		root->right = rightnode;

		return;
    }

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//checking for 'negation and' case.
	for(i=0; i<type.size(); i++){
		if(type[i]==21){
			flag = 1;
			break;
		}
	}
	if(flag==1){

		char ch0 = (((temp1.expr)[i]).str)[1];
		char ch1 = (((temp1.expr)[i]).str)[2];
		int index = 26*(ch0-'A')+ch1-'A';
		string tmp = map[index]->expr;
		string output = ((temp1.expr)[i]).str;

		int j;
		for(j=0; j<tmp.size(); j++)
			if(tmp[j]=='.')
				break;

		string tmp1 = tmp.substr(0,j);
		string tmp2 = tmp.substr(j+1,tmp.size()-j);
		
		string tmp3 = "-";
		tmp3 = tmp3.append(tmp1);

		string tmp4 = "-";
		tmp4 = tmp4.append(tmp2);

		tmp1 = tmp3;
		tmp2 = tmp4;

		((temp1.expr)[i]).str = tmp1;
		node *leftnode = new node;
		*leftnode = temp1;
		leftnode->left = NULL;
	    leftnode->right = NULL;
		leftnode->center = NULL;
		leftnode->parent = root;
		extendtree(leftnode);

		((temp1.expr)[i]).str = tmp2;
		node *rightnode = new node;
		*rightnode = temp1;
		rightnode->left = NULL;
	    rightnode->right = NULL;
		rightnode->center = NULL;
		rightnode->parent = root;
		extendtree(rightnode);

		string finalcomment = "Rule 2 applied on : ";
		String = output;
		getexpression();
		finalcomment.append(String);
		finalcomment.append("\n\tResultant elements : ");
		String = tmp1;
		getexpression();
		finalcomment.append(String);
		finalcomment.append(" , ");
		String = tmp2;
		getexpression();
		finalcomment.append(String);

		root->comment = temp1.comment = finalcomment;

		root->center = NULL;
		root->left = leftnode;
		root->right = rightnode;

		return;
    }

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//checking for 'equivalence' case.
	for(i=0; i<type.size(); i++){
		if(type[i]==14){
			flag = 1;
			break;
		}
	}
	if(flag==1){

		string tmp = ((temp1.expr)[i]).str;
		string output = tmp;;

		int j;
		for(j=0; j<tmp.size(); j++)
			if(tmp[j]=='=')
				break;


		string tmp1 = tmp.substr(0,j);
		string tmp2 = tmp.substr(j+1,tmp.size()-j);

		string tmp3 = tmp1;
		tmp3 = tmp3.append(".");
		tmp3 = tmp3.append(tmp2);

		string tmp4 = "-";
		tmp4 = tmp4.append(tmp1);
		tmp4 = tmp4.append(".-");
		tmp4 = tmp4.append(tmp2);

		tmp1 = tmp3;
		tmp2 = tmp4;

		((temp1.expr)[i]).str = tmp1;
		node *leftnode = new node;
		*leftnode = temp1;
		leftnode->left = NULL;
	    leftnode->right = NULL;
		leftnode->center = NULL;
		leftnode->parent = root;
		extendtree(leftnode);

		((temp1.expr)[i]).str = tmp2;
		node *rightnode = new node;
		*rightnode = temp1;
		rightnode->left = NULL;
	    rightnode->right = NULL;
		rightnode->center = NULL;
		rightnode->parent = root;
		extendtree(rightnode);

		string finalcomment = "Rule 8 applied on : ";
		String = output;
		getexpression();
		finalcomment.append(String);
		finalcomment.append("\n\tResultant elements : ");
		String = tmp1;
		getexpression();
		finalcomment.append(String);
		finalcomment.append(" , ");
		String = tmp2;
		getexpression();
		finalcomment.append(String);

		root->comment = temp1.comment = finalcomment;
		root->center = NULL;
		root->left = leftnode;
		root->right = rightnode;

		return;
    }


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//checking for 'negation equivalence' case.
	for(i=0; i<type.size(); i++){
		if(type[i]==24){
			flag = 1;
			break;
		}
	}
	if(flag==1){

		char ch0 = (((temp1.expr)[i]).str)[1];
		char ch1 = (((temp1.expr)[i]).str)[2];
		int index = 26*(ch0-'A')+ch1-'A';
		string tmp = map[index]->expr;
		string output = ((temp1.expr)[i]).str;

		int j;
		for(j=0; j<tmp.size(); j++)
			if(tmp[j]=='=')
				break;

		string tmp0 = "-";
		string tmp1 = tmp.substr(0,j);
		string tmp2 = tmp.substr(j+1,tmp.size()-j);

		string tmp3 = tmp1;
		tmp3 = tmp3.append(".-");
		tmp3 = tmp3.append(tmp2);

		string tmp4 = tmp0;
		tmp4 = tmp4.append(tmp1);
		tmp4 = tmp4.append(".");
		tmp4 = tmp4.append(tmp2);

		tmp1 = tmp3;
		tmp2 = tmp4;

		((temp1.expr)[i]).str = tmp1;
		exprnode* expression = new exprnode;
		expression->str = tmp2;
		(temp1.expr).push_back(*expression);

		((temp1.expr)[i]).str = tmp1;
		node *leftnode = new node;
		*leftnode = temp1;
		leftnode->left = NULL;
	    leftnode->right = NULL;
		leftnode->center = NULL;
		leftnode->parent = root;
		extendtree(leftnode);

		((temp1.expr)[i]).str = tmp2;
		node *rightnode = new node;
		*rightnode = temp1;
		rightnode->left = NULL;
	    rightnode->right = NULL;
		rightnode->center = NULL;
		rightnode->parent = root;
		extendtree(rightnode);

		string finalcomment = "Rule 9 applied on : ";
		String = output;
		getexpression();
		finalcomment.append(String);
		finalcomment.append("\n\tResultant elements : ");
		String = tmp1;
		getexpression();
		finalcomment.append(String);
		finalcomment.append(" , ");
		String = tmp2;
		getexpression();
		finalcomment.append(String);

		root->comment = temp1.comment = finalcomment;
		root->center = NULL;
		root->left = leftnode;
		root->right = rightnode;

		return;
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //setting all the literals at the end nodes.
    string output;
    for(int j=0;j<temp1.literals.size();j++){

		if(j==0){
	    	if(((temp1.literals)[j]).ltrl[0]=='-'){
	    		sprintf(buffer,"\n\tLiterals : %c%c",((temp1.literals)[j]).ltrl[0],((temp1.literals)[j]).ltrl[1]);
	    	}
	    	else{
	    		sprintf(buffer,"\n\tLiterals : %c",((temp1.literals)[j]).ltrl[1]);
	    	}

    	}
    	else{
    		if(((temp1.literals)[j]).ltrl[0]=='-'){
	    		sprintf(buffer,", %c%c",((temp1.literals)[j]).ltrl[0],((temp1.literals)[j]).ltrl[1]);
    		}
	    	else{
	    		sprintf(buffer,", %c",((temp1.literals)[j]).ltrl[1]);	
	    	}
    	}	
    	string temp(buffer);
	   	output.append(temp);
    }
    (root->comment).append(output);

}

//**************************************************************************************************************************//
//**************************************************************************************************************************//
//**************************************************************************************************************************//

void findchildren(node* root){
	if(root){
		if(root->left == NULL && root->right == NULL && root->center == NULL){
			pathscount++;
			if(root->close == 1)
				closepaths.push_back(root);
			else if(root->close == 0)
				openpaths.push_back(root);
			return;
		}
		findchildren(root->center);
		findchildren(root->left);
		findchildren(root->right);
	}
}

//*****************************************************************************************************************//

void printpaths(){

	vector<string> data;
	node *temp;

	cout<<endl<<"***************************************************************************************************"<<endl;

	cout<<endl<<"Total paths : "<<pathscount<<endl;

	cout<<endl<<"***************************************************************************************************"<<endl;

	cout<<endl<<"Number of open paths : "<<openpaths.size()<<endl;
	for(int i=0;i<openpaths.size();i++){
		data.clear();
		temp = openpaths[i];
		while(temp){
			data.push_back(temp->comment);
			temp = temp->parent;
		}
		cout<<endl<<"Open path "<<i+1<<" :"<<endl;
		for(int j=data.size()-1; j>=0; j--){
			cout<<data[j]<<endl;
		}
	}

	cout<<endl<<"***************************************************************************************************"<<endl;

	cout<<endl<<"Number of close paths : "<<closepaths.size()<<endl;
	for(int i=0;i<closepaths.size();i++){
		data.clear();
		temp = closepaths[i];
		while(temp){
			data.push_back(temp->comment);
			temp = temp->parent;
		}
		cout<<endl<<"Close path "<<i+1<<" :"<<endl;
		for(int j=data.size()-1; j>=0; j--){
			cout<<data[j]<<endl;
		}
	}
	cout<<endl<<"***************************************************************************************************"<<endl;
}

//*****************************************************************************************************************//

int printresult(){
	int openpathscount =  openpaths.size();
	int closepathscount =  closepaths.size();

	if(openpathscount==0 && closepathscount==0)
		cout<<endl<<"Either no string passed, or something passed wrong."<<endl;
	else if(openpathscount==0 && closepathscount!=0)
		cout<<endl<<"Entered clause is inconsistent."<<endl;
	else if(openpathscount!=0){
		cout<<endl<<"Entered clause is consistent."<<endl;
		return 1;
	}

	return 0;
}

//*****************************************************************************************************************//
//*****************************************************************************************************************//
//*****************************************************************************************************************//
