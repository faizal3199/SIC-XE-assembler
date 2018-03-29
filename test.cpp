#include<iostream>
#include<string>
// #include<sstream>
// #include<iomanip>
// #include<algorithm>

using namespace std;

string lol(){
  return "z";
}
int main(){
  string s;
  s = "a";
  s += (true)?("b"+lol()):"c";
  cout<<s<<endl;
}
