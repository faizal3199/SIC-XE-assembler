#include<iostream>
#include<fstream>
#include<string>
#include<algorithm>
#include<map>

using namespace std;
////////////////////////////TABLES.cpp////////////////////////////
struct struct_opcode{
    string opcode;
    int format;
    char exists;
    struct_opcode(){
      opcode="undefined";
      format=0;
      exists='n';
    }
};
struct struct_label{
     string address;
     string name;
     char exists;
     struct_label(){
       name="undefined";
       address="0";
       exists='n';
     }
};

typedef map<string,struct_label> SYMBOL_TABLE_TYPE;
typedef map<string,struct_opcode> OPCODE_TABLE_TYPE;

SYMBOL_TABLE_TYPE SYMTAB;
OPCODE_TABLE_TYPE OPTAB;

void load_OPTAB(){
  OPTAB["ADD"].opcode="18";
  OPTAB["ADD"].format=3;
  OPTAB["ADD"].exists='y';

  OPTAB["ADDF"].opcode="58";
  OPTAB["ADDF"].format=3;
  OPTAB["ADDF"].exists='y';

  OPTAB["ADDR"].opcode="90";
  OPTAB["ADDR"].format=2;
  OPTAB["ADDR"].exists='y';

  OPTAB["AND"].opcode="40";
  OPTAB["AND"].format=3;
  OPTAB["AND"].exists='y';

  OPTAB["CLEAR"].opcode="B4";
  OPTAB["CLEAR"].format=2;
  OPTAB["CLEAR"].exists='y';

  OPTAB["COMP"].opcode="28";
  OPTAB["COMP"].format=3;
  OPTAB["COMP"].exists='y';

  OPTAB["COMPF"].opcode="88";
  OPTAB["COMPF"].format=3;
  OPTAB["COMPF"].exists='y';

  OPTAB["COMPR"].opcode="A0";
  OPTAB["COMPR"].format=2;
  OPTAB["COMPR"].exists='y';

  OPTAB["DIV"].opcode="24";
  OPTAB["DIV"].format=3;
  OPTAB["DIV"].exists='y';

  OPTAB["DIVF"].opcode="64";
  OPTAB["DIVF"].format=3;
  OPTAB["DIVF"].exists='y';

  OPTAB["DIVR"].opcode="9C";
  OPTAB["DIVR"].format=2;
  OPTAB["DIVR"].exists='y';

  OPTAB["FIX"].opcode="C4";
  OPTAB["FIX"].format=1;
  OPTAB["FIX"].exists='y';

  OPTAB["FLOAT"].opcode="C0";
  OPTAB["FLOAT"].format=1;
  OPTAB["FLOAT"].exists='y';

  OPTAB["HIO"].opcode="F4";
  OPTAB["HIO"].format=1;
  OPTAB["HIO"].exists='y';

  OPTAB["J"].opcode="3C";
  OPTAB["J"].format=3;
  OPTAB["J"].exists='y';

  OPTAB["JEQ"].opcode="30";
  OPTAB["JEQ"].format=3;
  OPTAB["JEQ"].exists='y';

  OPTAB["JGT"].opcode="34";
  OPTAB["JGT"].format=3;
  OPTAB["JGT"].exists='y';

  OPTAB["JLT"].opcode="38";
  OPTAB["JLT"].format=3;
  OPTAB["JLT"].exists='y';

  OPTAB["JSUB"].opcode="48";
  OPTAB["JSUB"].format=3;
  OPTAB["JSUB"].exists='y';

  OPTAB["LDA"].opcode="00";
  OPTAB["LDA"].format=3;
  OPTAB["LDA"].exists='y';

  OPTAB["LDB"].opcode="68";
  OPTAB["LDB"].format=3;
  OPTAB["LDB"].exists='y';

  OPTAB["LDCH"].opcode="50";
  OPTAB["LDCH"].format=3;
  OPTAB["LDCH"].exists='y';

  OPTAB["LDF"].opcode="70";
  OPTAB["LDF"].format=3;
  OPTAB["LDF"].exists='y';

  OPTAB["LDL"].opcode="08";
  OPTAB["LDL"].format=3;
  OPTAB["LDL"].exists='y';

  OPTAB["LDS"].opcode="6C";
  OPTAB["LDS"].format=3;
  OPTAB["LDS"].exists='y';

  OPTAB["LDT"].opcode="74";
  OPTAB["LDT"].format=3;
  OPTAB["LDT"].exists='y';

  OPTAB["LDX"].opcode="04";
  OPTAB["LDX"].format=3;
  OPTAB["LDX"].exists='y';

  OPTAB["LPS"].opcode="D0";
  OPTAB["LPS"].format=3;
  OPTAB["LPS"].exists='y';

  OPTAB["MUL"].opcode="20";
  OPTAB["MUL"].format=3;
  OPTAB["MUL"].exists='y';

  OPTAB["MULF"].opcode="60";
  OPTAB["MULF"].format=3;
  OPTAB["MULF"].exists='y';

  OPTAB["MULR"].opcode="98";
  OPTAB["MULR"].format=2;
  OPTAB["MULR"].exists='y';

  OPTAB["NORM"].opcode="C8";
  OPTAB["NORM"].format=1;
  OPTAB["NORM"].exists='y';

  OPTAB["OR"].opcode="44";
  OPTAB["OR"].format=3;
  OPTAB["OR"].exists='y';

  OPTAB["RD"].opcode="D8";
  OPTAB["RD"].format=3;
  OPTAB["RD"].exists='y';

  OPTAB["RMO"].opcode="AC";
  OPTAB["RMO"].format=2;
  OPTAB["RMO"].exists='y';

  OPTAB["RSUB"].opcode="4C";
  OPTAB["RSUB"].format=3;
  OPTAB["RSUB"].exists='y';

  OPTAB["SHIFTL"].opcode="A4";
  OPTAB["SHIFTL"].format=2;
  OPTAB["SHIFTL"].exists='y';

  OPTAB["SHIFTR"].opcode="A8";
  OPTAB["SHIFTR"].format=2;
  OPTAB["SHIFTR"].exists='y';

  OPTAB["SIO"].opcode="F0";
  OPTAB["SIO"].format=1;
  OPTAB["SIO"].exists='y';

  OPTAB["SSK"].opcode="EC";
  OPTAB["SSK"].format=3;
  OPTAB["SSK"].exists='y';

  OPTAB["STA"].opcode="0C";
  OPTAB["STA"].format=3;
  OPTAB["STA"].exists='y';

  OPTAB["STB"].opcode="78";
  OPTAB["STB"].format=3;
  OPTAB["STB"].exists='y';

  OPTAB["STCH"].opcode="54";
  OPTAB["STCH"].format=3;
  OPTAB["STCH"].exists='y';

  OPTAB["STF"].opcode="80";
  OPTAB["STF"].format=3;
  OPTAB["STF"].exists='y';

  OPTAB["STI"].opcode="D4";
  OPTAB["STI"].format=3;
  OPTAB["STI"].exists='y';

  OPTAB["STL"].opcode="14";
  OPTAB["STL"].format=3;
  OPTAB["STL"].exists='y';

  OPTAB["STS"].opcode="7C";
  OPTAB["STS"].format=3;
  OPTAB["STS"].exists='y';

  OPTAB["STSW"].opcode="E8";
  OPTAB["STSW"].format=3;
  OPTAB["STSW"].exists='y';

  OPTAB["STT"].opcode="84";
  OPTAB["STT"].format=3;
  OPTAB["STT"].exists='y';

  OPTAB["STX"].opcode="10";
  OPTAB["STX"].format=3;
  OPTAB["STX"].exists='y';

  OPTAB["SUB"].opcode="1C";
  OPTAB["SUB"].format=3;
  OPTAB["SUB"].exists='y';

  OPTAB["SUBF"].opcode="5C";
  OPTAB["SUBF"].format=3;
  OPTAB["SUBF"].exists='y';

  OPTAB["SUBR"].opcode="94";
  OPTAB["SUBR"].format=2;
  OPTAB["SUBR"].exists='y';

  OPTAB["SVC"].opcode="B0";
  OPTAB["SVC"].format=2;
  OPTAB["SVC"].exists='y';

  OPTAB["TD"].opcode="E0";
  OPTAB["TD"].format=3;
  OPTAB["TD"].exists='y';

  OPTAB["TIO"].opcode="F8";
  OPTAB["TIO"].format=1;
  OPTAB["TIO"].exists='y';

  OPTAB["TIX"].opcode="2C";
  OPTAB["TIX"].format=3;
  OPTAB["TIX"].exists='y';

  OPTAB["TIXR"].opcode="B8";
  OPTAB["TIXR"].format=2;
  OPTAB["TIXR"].exists='y';

  OPTAB["WD"].opcode="DC";
  OPTAB["WD"].format=3;
  OPTAB["WD"].exists='y';
}
////////////////////////////TABLES.cpp////////////////////////////

////////////////////UTILITY.cpp/////////////////////////
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
////////////////////UTILITY.cpp/////////////////////////
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
void pass1(){
  ifstream sourceFile;//begin
  ofstream intermediateFile, errorFile;

  sourceFile.open("test_input.txt");
  intermediateFile.open("intermediate_file.txt");
  writeToFile(intermediateFile,"Line\tAddress\tLabel\tOPCODE\tOPERAND\tComment");
  errorFile.open("error_file.txt");

  string fileLine;
  string writeData;
  int index=0;

  getline(sourceFile,fileLine);
  while(checkCommentLine(fileLine)){
    getline(sourceFile,fileLine);//read first input line
  }

  bool statusCode;
  string label,opcode,operand,comment;

  readFirstNonWhiteSpace(fileLine,index,statusCode,label);
  readFirstNonWhiteSpace(fileLine,index,statusCode,opcode);

  int startAddress,LOCCTR,lineNumber,lastDeltaLOCCTR;
  lineNumber = 5;
  lastDeltaLOCCTR = 0;

  if(opcode=="START"){
    readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
    readFirstNonWhiteSpace(fileLine,index,statusCode,comment,true);
    startAddress = stringHexToInt(operand);
    // cout<<startAddress<<endl;
    // exit(0);
    LOCCTR = startAddress;
    writeData = to_string(lineNumber) + "\t" + intToStringHex(LOCCTR-lastDeltaLOCCTR) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment;
    writeToFile(intermediateFile,writeData); // Write file to intermediate file

    getline(sourceFile,fileLine); //Read next line
    lineNumber += 5;
    index = 0;
    readFirstNonWhiteSpace(fileLine,index,statusCode,label); //Parse label
    readFirstNonWhiteSpace(fileLine,index,statusCode,opcode);//Parse OPCODE
  }
  else{
    startAddress = 0;
    LOCCTR = 0;
  }

  while(opcode!="END"){
    if(!checkCommentLine(fileLine)){
      if(label!=""){//Label found
        if(SYMTAB[label].exists=='n'){
          SYMTAB[label].name = label;
          SYMTAB[label].address = intToStringHex(LOCCTR);
          SYMTAB[label].exists = 'y';
        }
        else{
          writeData = "Line: "+to_string(lineNumber)+" : Duplicate symbol for '"+label+"'. Previously defined at "+SYMTAB[label].address;
          writeToFile(errorFile,writeData);
        }
      }
      if(OPTAB[getRealOpcode(opcode)].exists=='y'){//Search for opcode in OPTAB
        readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
        if(OPTAB[getRealOpcode(opcode)].format==3){
          LOCCTR += 3;
          lastDeltaLOCCTR += 3;
          if(getOpcodeFormat(opcode)=='+'){
            LOCCTR += 1;
            lastDeltaLOCCTR += 3;
          }
        }
        else{
          LOCCTR += OPTAB[getRealOpcode(opcode)].format;
          lastDeltaLOCCTR += OPTAB[getRealOpcode(opcode)].format;
        }
      }
      else if(opcode == "WORD"){
        readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
        LOCCTR += 3;
        lastDeltaLOCCTR += 3;
      }
      else if(opcode == "RESW"){
        readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
        LOCCTR += 3*stoi(operand);
        lastDeltaLOCCTR += 3*stoi(operand);
      }
      else if(opcode == "RESB"){
        readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
        LOCCTR += stoi(operand);
        lastDeltaLOCCTR += stoi(operand);
      }
      else if(opcode == "BYTE"){
        readByteOperand(fileLine,index,statusCode,operand);
        if(operand[0]=='X'){
          LOCCTR += (operand.length() -3)/2;
          lastDeltaLOCCTR += (operand.length() -3)/2;
        }
        else if(operand[0]=='C'){
          LOCCTR += operand.length() -3;
          lastDeltaLOCCTR += operand.length() -3;
        }
        // else{
        //   writeData = "Line: "+to_string(line)+" : Invalid operand for BYTE. Found " + operand;
        //   writeToFile(errorFile,writeData);
        // }
      }
      else{
        readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
        writeData = "Line: "+to_string(lineNumber)+" : Invalid OPCODE. Found " + opcode;
        writeToFile(errorFile,writeData);
      }
      readFirstNonWhiteSpace(fileLine,index,statusCode,comment,true);
      writeData = to_string(lineNumber) + "\t" + intToStringHex(LOCCTR-lastDeltaLOCCTR) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment;
    }
    else{
      writeData = to_string(lineNumber) + "\t" + fileLine;
    }
    writeToFile(intermediateFile,writeData);

    getline(sourceFile,fileLine); //Read next line
    lineNumber += 5;
    index = 0;
    lastDeltaLOCCTR = 0;
    readFirstNonWhiteSpace(fileLine,index,statusCode,label); //Parse label
    readFirstNonWhiteSpace(fileLine,index,statusCode,opcode);//Parse OPCODE
  }
  readFirstNonWhiteSpace(fileLine,index,statusCode,operand);
  readFirstNonWhiteSpace(fileLine,index,statusCode,comment,true);
  writeData = to_string(lineNumber) + "\t" + intToStringHex(LOCCTR-lastDeltaLOCCTR) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment;
  writeToFile(intermediateFile,writeData);

  int program_length = LOCCTR - startAddress;
}

int main(){
  load_OPTAB();
  pass1();
  return 0;
}
