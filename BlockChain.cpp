#include <bits/stdc++.h>
#include <functional> // Required for std::hash
using namespace std;

class transaction{
    string sender;
    string receiver;
    double amount;
    string timestamp; // Optional, can be added later
    string transactionID; // Optional, can be added later
    string signature;

public:
    transaction(string s, string r, double a) : sender(s), receiver(r), amount(a) {
        timestamp = to_string(time(nullptr)); // Current time as a string
        transactionID = getHash(); // Generate a unique ID based on the transaction details
        signature = ""; // Placeholder for signature, can be implemented later
    }
    string getSender() const { return sender; }
    string getReceiver() const { return receiver; }
    double getAmount() const { return amount; }

    string getHash() const{
        // Simple hash function for demonstration purposes
        return to_string(hash<string>()(sender + receiver + to_string(amount) + timestamp + signature));
    }
};

class block{
    int index; // Static variable to keep track of the block index
    vector<transaction> transactions;
    int nonce;
    string previousHash;
    string blockHash;
    string timestamp;

public:
    block(int idx, const string& prevHash, const vector<transaction> transactions) : index(idx), previousHash(prevHash), nonce(0), transactions(transactions) {
        timestamp = to_string(time(nullptr)); // Current time as a string
        blockHash = getHash(); // Generate the block hash
    }
    string getHash() const{
        // Simple hash function for demonstration purposes
        string combine = to_string(index) + previousHash + timestamp + to_string(nonce);
        for (const auto& tx : transactions) {
            combine += tx.getHash();
        }
        return to_string(hash<string>()(combine));
    }
    string getBlockHash() const {
        return blockHash;
    }
    string getPreviousHash() const {
        return previousHash;
    }
    vector<transaction> getTransactions() const {
        return transactions;
    }
    string getTimestamp() const {
        return timestamp;
    }
};

class blockchain{
    // int size;
    vector<block> chain;

public:
    blockchain(int diff) {
        chain.push_back(block(0, "0", {})); // Genesis block
    }

    block getLatestBlock() const {
        return chain.back(); // Return the last block in the chain
    }

    // add block
    void addBlock(vector<transaction> transactions) {
        const block& latestBlock = getLatestBlock();
        block newBlock(chain.size(), latestBlock.getBlockHash(), transactions);
        chain.push_back(newBlock); // Add the new block to the chain
    }

    // print chain
    void printChain() const {
        for (const auto& blk : chain) {
            cout << "Block Index: " << blk.getBlockHash() << endl;
            cout << "Previous Hash: " << blk.getPreviousHash() << endl;
            cout << "Timestamp: " << blk.getTimestamp() << endl;
            cout << "Transactions: " << endl;
            // Here you can iterate through transactions in the block if needed
            for(const auto& tx : blk.getTransactions()) {
                cout << "  Sender: " << tx.getSender() << ", Receiver: " << tx.getReceiver() << ", Amount: " << tx.getAmount() << endl;
            }
            cout << "------------------------" << endl;
        }
    }
};

int main() {
    blockchain myBlockchain(2); // Create a blockchain with difficulty 2

    // Create some transactions
    transaction tx1("Alice", "Bob", 50.0);
    transaction tx2("Bob", "Charlie", 30.0);

    // Add a block with these transactions
    myBlockchain.addBlock({tx1, tx2});

    // Print the blockchain
    myBlockchain.printChain();

    return 0;
}