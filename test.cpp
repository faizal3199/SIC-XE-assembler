#include<iostream>
#include<string>
#include<sstream>
#include<iomanip>
#include<algorithm>

using namespace std;

string string_to_hex(const string& input){
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
int main(){
  string x="a";
  string y = "b";
  cin>>x;
  cout<<(x+y).length()<<endl;
}
