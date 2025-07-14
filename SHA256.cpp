#include<bits/stdc++.h>
using namespace std;

// max string length that can be hashed should be definable 64 bits
// max length possible 2^64-1

vector<uint8_t> padding(string &input) {
    vector<uint8_t> paddedString(input.begin(), input.end());
    // adding 10000000 (8 bits -> 1 byte)
    paddedString.push_back(0x80);

    // we want paddedMessage.size() % 64 == 56 (bytes)
    while(paddedString.size()%64!=56)
        paddedString.push_back(0x00);
    
    // length in bits for original string stored in 64 bits/8 bytes
    uint64_t bitLength = input.size()*8;
    for (int i = 7; i >= 0; --i) {
        paddedString.push_back((bitLength >> (i * 8)) & 0xFF);
    }
    return paddedString;
}

void fxn(vector<uint8_t> paddedString){

}

string SHA256(string s){
    vector<uint8_t> paddedString = padding(s);
    cout<<"paddedString size:"<<paddedString.size()<<endl;
    for(uint8_t c:paddedString) cout<<int(c)<<"|";
    cout<<endl;
    return "hashed string";
}

int main(){
    string s = "jainish";
    cout<<SHA256(s)<<endl;;
}