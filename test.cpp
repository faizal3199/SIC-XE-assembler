#include<iostream>
#include<string>
#include<map>

using namespace std;

struct opcode_struct{
	string name;
	int format;
	char exists;
	opcode_struct(){
		exists = 'n';
		name = "sdf";
	}
};

typedef map<string,opcode_struct> optab;
optab temp2;
temp2["fa"].name="faizal1";
int main(){

	// opcode_struct temp = {"faizal",6,'y'};
	string x;
	x = "10";
	x = 'z' + x;
	// cout<<temp.format<<endl;s
	cout<<temp2["fas"].name<<endl;
	return 0;
}
