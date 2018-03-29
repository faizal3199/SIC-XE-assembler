/*This file contains function helpfull for functioningof other files
Function declared here rarelly changes*/
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
