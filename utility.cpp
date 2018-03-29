/*This file contains function helpfull for functioningof other files
Function declared here rarelly changes
This file also contains important files to #include*/
#include<iostream>
#include<fstream>
#include<string>
#include<map>

using namespace std;

char intToHexChar(int a){
  if(a==0) return '0';
  if(a==1) return '1';
  if(a==2) return '2';
  if(a==3) return '3';
  if(a==4) return '4';
  if(a==5) return '5';
  if(a==6) return '6';
  if(a==7) return '7';
  if(a==8) return '8';
  if(a==9) return '9';
  if(a==10) return 'A';
  if(a==11) return 'B';
  if(a==12) return 'C';
  if(a==13) return 'D';
  if(a==14) return 'E';
  if(a==15) return 'F';
}

int charHexToInt(char a){
  if(a=='0') return 0;
  if(a=='1') return 1;
  if(a=='2') return 2;
  if(a=='3') return 3;
  if(a=='4') return 4;
  if(a=='5') return 5;
  if(a=='6') return 6;
  if(a=='7') return 7;
  if(a=='8') return 8;
  if(a=='9') return 9;
  if(a=='A' || a=='a') return 10;
  if(a=='B' || a=='b') return 11;
  if(a=='C' || a=='c') return 12;
  if(a=='D' || a=='d') return 13;
  if(a=='E' || a=='e') return 14;
  if(a=='F' || a=='f') return 15;
}

string intToStringHex(int x){
  string value = "";
  while(x>0){
    value = intToHexChar(x%16) + value;
    x = x/16;
  }
  return value;
}

int stringHexToInt(string x){
  int value =0;
  int i = 0;
  while(i<x.length()){
    value = value*16 + charHexToInt(x[i]);
    i++;
  }
  return value;
}

bool checkWhiteSpace(char x){
  if(x==' ' || x=='\t'){
    return true;
  }
  return false;
}

bool checkCommentLine(string line){
  if(line[0]=='.'){
    return true;
  }
  return false;
}

void readFirstNonWhiteSpace(string line,int& index,bool& status,string& data,bool readTillEnd=false){
  data = "";
  status = true;
  if(readTillEnd){
    data = line.substr(index,line.length() - index);
    if(data==""){
      status = false;
    }
    return;
  }
  while(index<line.length()&&!checkWhiteSpace(line[index])){//If no whitespace then data
    data += line[index];
    index++;
  }

  if(data==""){
    status = false;
  }

  while(index<line.length()&&checkWhiteSpace(line[index])){//Increase index to pass all whitespace
    index++;
  }
}

void readByteOperand(string line,int& index,bool& status,string& data){
  data = "";
  status = true;
  if(line[index]=='C'){
    data += line[index++];
    char identifier = line[index++];
    data += identifier;
    while(index<line.length() && line[index]!=identifier){//Copy all data until next identifier regardless of whitespace
      data += line[index];
      index++;
    }
    data += identifier;
  }
  else{
    while(index<line.length()&&!checkWhiteSpace(line[index])){//In no whitespace then data
      data += line[index];
      index++;
    }
  }

  if(data==""){
    status = false;
  }

  while(index<line.length()&&checkWhiteSpace(line[index])){//Increase index to pass all whitespace
    index++;
  }
}

void writeToFile(ofstream& file,string data){
  file<<data<<endl;
}

string getRealOpcode(string opcode){
  if(opcode[0] == '+' || opcode[0] == '@'){
    return opcode.substr(1,opcode.length() - 1);
  }
  return opcode;
}

char getOpcodeFormat(string opcode){
  if(opcode[0] == '+' || opcode[0] == '@'){
    return opcode[0];
  }
  return ' ';
}
