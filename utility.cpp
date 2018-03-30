/*This file contains function helpfull for functioningof other files
Function declared here rarelly changes
This file also contains important files to #include*/
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<iomanip>
#include<map>
#include<algorithm>

using namespace std;

// char intToHexChar(int a){
//   if(a==0) return '0';
//   if(a==1) return '1';
//   if(a==2) return '2';
//   if(a==3) return '3';
//   if(a==4) return '4';
//   if(a==5) return '5';
//   if(a==6) return '6';
//   if(a==7) return '7';
//   if(a==8) return '8';
//   if(a==9) return '9';
//   if(a==10) return 'A';
//   if(a==11) return 'B';
//   if(a==12) return 'C';
//   if(a==13) return 'D';
//   if(a==14) return 'E';
//   if(a==15) return 'F';
// }

// int charHexToInt(char a){
//   if(a=='0') return 0;
//   if(a=='1') return 1;
//   if(a=='2') return 2;
//   if(a=='3') return 3;
//   if(a=='4') return 4;
//   if(a=='5') return 5;
//   if(a=='6') return 6;
//   if(a=='7') return 7;
//   if(a=='8') return 8;
//   if(a=='9') return 9;
//   if(a=='A' || a=='a') return 10;
//   if(a=='B' || a=='b') return 11;
//   if(a=='C' || a=='c') return 12;
//   if(a=='D' || a=='d') return 13;
//   if(a=='E' || a=='e') return 14;
//   if(a=='F' || a=='f') return 15;
// }

string intToStringHex(int x,int fill = 5){
  stringstream s;
  s << setfill('0') << setw(fill) << hex << x;
  string temp = s.str();
  temp = temp.substr(temp.length()-fill,fill);
  transform(temp.begin(), temp.end(),temp.begin(),::toupper);
  return temp;
}

string expandString(string data,int length,char fillChar,bool back=false){
  if(back){
    if(length<=data.length()){
      return data.substr(0,length);
    }
    else{
      for(int i = length-data.length();i>0;i--){
        data += fillChar;
      }
    }
  }
  else{
    if(length<=data.length()){
      return data.substr(data.length()-length,length);
    }
    else{
      for(int i = length-data.length();i>0;i--){
        data = fillChar + data;
      }
    }
  }
  return data;
}
int stringHexToInt(string x){
  return stoul(x,nullptr,16);
}
//   int value =0;
//   int i = 0;
//   while(i<x.length()){
//     value = value*16 + charHexToInt(x[i]);
//     i++;
//   }
//   return value;
// } replaced with stoul supported from c++v11

string stringToHexString(const string& input){
    static const char* const lut = "0123456789ABCDEF";
    size_t len = input.length();

    string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i)
    {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
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

bool if_all_num(string x){
  bool all_num = true;
  int i = 0;
  while(all_num && (i<x.length())){
    all_num &= isdigit(x[i++]);
  }
  return all_num;
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

void writeToFile(ofstream& file,string data,bool newline=true){
  if(newline){
    file<<data<<endl;
  }else{
    file<<data;
  }
}

string getRealOpcode(string opcode){
  if(opcode[0] == '+' || opcode[0] == '@'){
    return opcode.substr(1,opcode.length() - 1);
  }
  return opcode;
}

char getFlagFormat(string opcode){
  if(opcode[0] == '#' || opcode[0] == '+' || opcode[0] == '@'){
    return opcode[0];
  }
  return ' ';
}
