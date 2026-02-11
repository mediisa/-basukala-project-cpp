#include "common.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <unordered_map>


std::vector<User> users;
std::vector<Product> products;
std::vector<OrderRecord> orders;
std::vector<Package> packages;

long long globalPackageSequence = 0;
int orderSequence = 0;
int packageSequence = 0;


const double SHIPPING_COST = 100.0;
const std::string USERS_FILE = "users.txt";
const std::string PRODUCTS_FILE = "products.txt";
const std::string ORDERS_FILE = "orders.txt";
const std::string PACKAGES_FILE = "packages.txt";


const std::vector<char> WAREHOUSE_CITIES = {'B', 'I'};
const std::unordered_map<char, std::string> CITY_LABELS = {
    {'A', "Bimsy"}, {'B', "Garous"}, {'C', "Debrecen"}, {'D', "Lae"},
    {'E', "Merribell"}, {'F', "Wimbie"}, {'G', "Aguascalientes"},
    {'H', "Froli"}, {'I', "Tainan"}, {'J', "Honara"},
    {'K', "Twinklehollow"}, {'L', "Trinkleby"}
};


std::string nextOrderId() {
    ++orderSequence;
    std::stringstream ss;
    ss << "ORD" << std::setw(5) << std::setfill('0') << orderSequence;
    return ss.str();
}

std::string nextPackageId() {
    ++packageSequence;
    std::stringstream ss;
    ss << "PKG" << std::setw(5) << std::setfill('0') << packageSequence;
    return ss.str();
}


TrieNode::TrieNode() : isWord(false), children(26, nullptr) {}

ProductTrie::ProductTrie() { root = new TrieNode(); }
ProductTrie::~ProductTrie() { clear(root); }

void ProductTrie::clear(TrieNode* node) {
    if (!node) return;
    for (auto child : node->children) clear(child);
    delete node;
}

void ProductTrie::insert(const std::string &word) {
    TrieNode* node = root;
    for (char c : word) {
        if (!isalpha(static_cast<unsigned char>(c))) continue;
        int idx = toupper(c) - 'A';
        if (!node->children[idx]) node->children[idx] = new TrieNode();
        node = node->children[idx];
    }
    node->isWord = true;
}

void ProductTrie::build(const std::vector<Product> &plist) {
    clear(root);
    root = new TrieNode();
    for (const auto &p : plist) {
        std::string upperName = p.name;
        for (char &c : upperName) c = toupper(c);
        insert(upperName);
    }
}

void ProductTrie::collect(TrieNode* node, std::string current, std::vector<std::string> &results, size_t limit) {
    if (!node || results.size() >= limit) return;
    if (node->isWord) results.push_back(current);
    for (int i = 0; i < 26 && results.size() < limit; ++i) {
        if (node->children[i]) {
            collect(node->children[i], current + static_cast<char>('A' + i), results, limit);
        }
    }
}

std::vector<std::string> ProductTrie::suggest(const std::string &prefix, size_t limit) {
    TrieNode* node = root;
    std::string cleaned;
    for (char c : prefix) {
        if (!isalpha(static_cast<unsigned char>(c))) continue;
        cleaned.push_back(toupper(c));
    }
    for (char c : cleaned) {
        int idx = c - 'A';
        if (!node->children[idx]) return {};
        node = node->children[idx];
    }
    std::vector<std::string> results;
    collect(node, cleaned, results, limit);
    for (auto &res : results) {
        for (char &c : res) c = toupper(c);
    }
    return results;
}

bool ProductTrie::search(const std::string &word) {
    TrieNode* node = root;
    std::string cleaned;
    for (char c : word) {
        if (!isalpha(static_cast<unsigned char>(c))) continue;
        cleaned.push_back(toupper(c));
    }
    for (char c : cleaned) {
        int idx = c - 'A';
        if (!node->children[idx]) return false;
        node = node->children[idx];
    }
    return node->isWord;
}

bool ProductTrie::eraseHelper(TrieNode* node, const std::string &word, int depth) {
    if (!node) return false;
    if (depth == static_cast<int>(word.size())) {
        if (!node->isWord) return false;
        node->isWord = false;
        for (auto child : node->children) if (child) return false;
        return true;
    }
    int idx = word[depth] - 'A';
    if (eraseHelper(node->children[idx], word, depth + 1)) {
        delete node->children[idx];
        node->children[idx] = nullptr;
        if (node->isWord) return false;
        for (auto child : node->children) if (child) return false;
        return true;
    }
    return false;
}

void ProductTrie::erase(const std::string &word) {
    std::string cleaned;
    for (char c : word) {
        if (!isalpha(static_cast<unsigned char>(c))) continue;
        cleaned.push_back(toupper(c));
    }
    eraseHelper(root, cleaned, 0);
}



std::unordered_map<char, std::vector<std::pair<char, int>>> buildCityGraph() {
    std::unordered_map<char, std::vector<std::pair<char, int>>> graph;

    auto addEdge = [&](char u, char v, int w) {
        graph[u].push_back({v, w});
        graph[v].push_back({u, w});
    };

    addEdge('A', 'B', 2);
    addEdge('A', 'C', 5);
    addEdge('B', 'F', 5);
    addEdge('B', 'A', 2);
    addEdge('B', 'F', 5);
    addEdge('C', 'E', 4);
    addEdge('C', 'G', 6);
    addEdge('C', 'K', 4);
    addEdge('C', 'L', 3);
    addEdge('C', 'A', 5);
    addEdge('D', 'E', 1);
    addEdge('E', 'F', 2);
    addEdge('E', 'G', 3);
    addEdge('F', 'H', 3);
    addEdge('F', 'G', 5);
    addEdge('G', 'H', 4);
    addEdge('G', 'I', 2);
    addEdge('G', 'J', 4);
    addEdge('G', 'K', 10);
    addEdge('H', 'I', 4);
    addEdge('I', 'J', 2);
    addEdge('J', 'K', 8);
    addEdge('K', 'L', 3);

    return graph;
}
