#include<bits/stdc++.h>
using namespace std;

// max string length that can be hashed should be definable 64 bits
// max length possible 2^64-1

// ...point:2
// | merges bits without considering numeric value
// + combines values and spreads changes across bits (better diffusion)
// + enables cascading carry effect, crucial for SHA's avalanche property

// ...point:3
// σ -> small sigma/sum s
// Σ -> capital sigma/sum S

// ...point:4
// These are defined by the SHA-256 standard (FIPS PUB 180-4),
// derived from the first 32 bits of the fractional parts of the square roots of the first 8 primes (2–19).

// ...point:5
// Round Constants K[64], defined by the standard.
// These are the first 32 bits of the fractional parts of the cube roots of the first 64 primes (2–311).

vector<uint8_t> padding(string &input) {
    vector<uint8_t> paddedString(input.begin(), input.end());
    // adding 10000000 (8 bits -> 1 byte)
    paddedString.push_back(0x80);

    // we want paddedMessage.size() % 64 == 56 (bytes)
    while(paddedString.size()%64!=56)
        paddedString.push_back(0x00);
    
    // length in bits for original string stored in 64 bits/8 bytes
    uint64_t bitLength = input.size()*8;
    // uint8_t is unsigned, so when i = 0, --i becomes 255 (wraps around) -> infinite loop.
    for (int i = 7; i >= 0; --i) {
        paddedString.push_back((bitLength >> (i * 8)) & 0xFF);
    }
    return paddedString;
}

vector<uint32_t> generateMessageSchedule(vector<uint8_t> block64Bytes){
    // divide whole string into 64 bytes / 16 words blocks
    // 1 word = 4 bytes = 32 bits (so uint32_t)
    vector<uint32_t> words(64);
    // 0-15 (16 words) from padded string
    for(uint32_t i=0; i<16; i++){
        words[i] = uint32_t(block64Bytes[i*4]) << 24 |
                    uint32_t(block64Bytes[i*4 + 1]) << 16 |
                    uint32_t(block64Bytes[i*4 + 2]) << 8 |
                    uint32_t(block64Bytes[i*4 + 3]);
    }

    // 16-63 (48 words) other words
    // for each t, in 16-63
    // W[t] = σ1(W[t−2]) + W[t−7] + σ0(W[t−15]) + W[t−16]
    // where
    // σ0(W[t−15]) = ROTR^7(W[t−15]) XOR ROTR^18(W[t−15]) XOR SHR^3(W[t−15])
    // σ1(W[t−2]) = ROTR^17(W[t−2]) XOR ROTR^19(W[t−2]) XOR SHR^10(W[t−2])
    // ...point:2
    // ...point:3
    auto s0 = [](uint32_t word)->uint32_t{
        return ((word>>7) | (word<<25)) ^ ((word>>18) | (word<<14)) ^ (word>>3);
    };
    auto s1 = [](uint32_t word)->uint32_t{
        return ((word>>17) | (word<<15)) ^ ((word>>19) | (word<<13)) ^ (word>>10);
    };
    for(uint32_t i=16; i<64; i++){
        words[i] = s1(words[i-2]) + words[i-7] + s0(words[i-15]) + words[i-16];
    }

    return words;
}

void compressBlock(const vector<uint32_t>& w, const uint32_t* k, uint32_t* H){
    uint32_t a = H[0];
    uint32_t b = H[1];
    uint32_t c = H[2];
    uint32_t d = H[3];
    uint32_t e = H[4];
    uint32_t f = H[5];
    uint32_t g = H[6];
    uint32_t h = H[7];
    uint32_t t1, t2;

    // ...point:3
    // Ch(x,y,z) =	(x & y) ^ (~x & z) – chooses y or z depending on x
    // Maj(x,y,z) =	(x & y) ^ (x & z) ^ (y & z) – maj vote
    // Σ0(x) =	ROTR^2(x) ^ ROTR^13(x) ^ ROTR^22(x)
    // Σ1(x) =	ROTR^6(x) ^ ROTR^11(x) ^ ROTR^25(x)
    auto ch = [](uint32_t x, uint32_t y, uint32_t z)->uint32_t{
        return (x & y) ^ (~x & z);
    };
    auto maj = [](uint32_t x, uint32_t y, uint32_t z)->uint32_t{
        return (x & y) ^ (x & z) ^ (y & z);
    };
    auto S0 = [](uint32_t word)->uint32_t{
        return ((word>>2) | (word<<30)) ^ ((word>>13) | (word<<19)) ^ ((word>>22) | (word<<10));
    };
    auto S1 = [](uint32_t word)->uint32_t{
        return ((word>>6) | (word<<26)) ^ ((word>>11) | (word<<21)) ^ ((word>>25) | (word<<7));
    };

    for(uint32_t i=0; i<64; i++){
        t1 = h + S1(e) + ch(e, f, g) + k[i] + w[i];
        t2 = S0(a) + maj(a, b, c);

        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    H[0] += a;
    H[1] += b;
    H[2] += c;
    H[3] += d;
    H[4] += e;
    H[5] += f;
    H[6] += g;
    H[7] += h;

    // return H;
    // No return value. Just update H directly inside the function.
    // Slightly less memory/copy overhead
}

string SHA256(string s){
    vector<uint8_t> paddedString = padding(s);
    // cout<<"paddedString size:"<<paddedString.size()<<endl;
    // for(uint32_t c:paddedString) cout<<int(c)<<"|";
    // cout<<endl;

    // rn we have have short string, so single block of 64 bytes/512 bits
    vector<uint32_t> messageSchedule = generateMessageSchedule(paddedString);
    // cout<<"words / message schedules:"<<endl;
    // cout<<hex;
    // for(uint32_t i=0; i<64; i++){
        // if(i%8==0)  cout<<endl;
        // cout<<messageSchedule[i]<<" ";
    // }
    // cout<<endl;

    // initial hash
    // ...point:4
    uint32_t currentHash[8] = {
        0x6a09e667,
        0xbb67ae85,
        0x3c6ef372,
        0xa54ff53a,
        0x510e527f,
        0x9b05688c,
        0x1f83d9ab,
        0x5be0cd19
    };
    // key
    // ...point:5
    uint32_t K[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
        0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    compressBlock(messageSchedule, K, currentHash);

    // cout<<"Hash: "<<endl;
    // string hash="";
    // for(auto x:currentHash){
    //     cout<<x<<" ";
    // }
    // cout<<endl;

    ostringstream result;
    result << hex << setfill('0');

    for (uint32_t h : currentHash)
        result << setw(8) << h;

    return result.str();

    // return "hashed string";
}

int main(){
    string s = "";
    cout<<"enter string to hash: ";
    getline(cin, s);

    cout<<"Hash: "<<SHA256(s)<<endl;;
}

// jainish -> a97437861aacfa855ef82a47e368938a43a1747c1deda9263873d4514567ae19