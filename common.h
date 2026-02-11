#ifndef COMMIN_H
#define COMMIN_H

#include <iostream>
#include <vector>
#include <unordered_map>



extern int orderSequence;
extern int packageSequence;

struct OrderItem {
    std::string productName;
    int quantity = 0;
    double unitPrice = 0.0;
};

struct Product {
    std::string name;
    double price = 0.0;
    int stock = 0;
    int soldCount = 0;
};

struct OrderRecord {
    std::string id;
    std::string username;
    std::vector<OrderItem> items;
    char destinationCity = 'A';
    std::string address;
    std::string status = "Pending"; 
};

struct Package {
    std::string id;
    std::string orderId;
    std::string username;
    std::vector<OrderItem> items;
    char destinationCity = 'A';
    std::string address;
    int score = 0;
    std::string status = "Pending"; 
    std::string routeDisplay;
    int routeDistance = 0;
    long long enqueueIndex = 0; 
};

struct User {
    std::string username;
    std::string password;
    std::string role = "CUSTOMER";
    double balance = 0.0;
    int score = 0;
    std::vector<std::string> orderHistory;
};

struct TrieNode {
    bool isWord = false;
    std::vector<TrieNode*> children;
    TrieNode();
};

struct PathResult {
    std::vector<char> path;
    int distance = INT_MAX;
    bool reachable = false;
};

std::string nextOrderId();
std::string nextPackageId();

class ProductTrie {
private:
    TrieNode* root;
    void collect(TrieNode* node, std::string current, std::vector<std::string> &results, size_t limit);
    bool eraseHelper(TrieNode* node, const std::string &word, int depth);
    void clear(TrieNode* node);
public:
    ProductTrie();
    ~ProductTrie();

    void insert(const std::string &word);
    void build(const std::vector<Product> &products);
    std::vector<std::string> suggest(const std::string &prefix, size_t limit = 5);
    bool search(const std::string &word);
    void erase(const std::string &word);
};

std::unordered_map<char, std::vector<std::pair<char, int>>> buildCityGraph();

#endif