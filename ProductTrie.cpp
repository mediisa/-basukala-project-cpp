#include "ProductTrie.h"
#include <cctype>

ProductTrie productTrie;

TrieNode::TrieNode() : isWord(false), children(26, nullptr) {}

ProductTrie::ProductTrie() : root(new TrieNode()) {}

ProductTrie::~ProductTrie() {
    clear(root);
}

void ProductTrie::clear(TrieNode* node) {
    if (!node) return;
    for (auto child : node->children) {
        clear(child);
    }
    delete node;
}

std::string ProductTrie::normalize(const std::string& input) const {
    std::string cleaned;
    cleaned.reserve(input.size());
    for (char c : input) {
        if (!std::isalpha(static_cast<unsigned char>(c))) continue;
        cleaned.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
    }
    return cleaned;
}

void ProductTrie::insert(const std::string& word) {
    std::string cleaned = normalize(word);
    if (cleaned.empty()) return;

    TrieNode* node = root;
    for (char c : cleaned) {
        int idx = c - 'A';
        if (idx < 0 || idx >= 26) continue;
        if (!node->children[idx]) node->children[idx] = new TrieNode();
        node = node->children[idx];
    }
    node->isWord = true;
}

void ProductTrie::build(const std::vector<Product>& products) {
    clear(root);
    root = new TrieNode();

    for (const auto& p : products) {
        insert(p.name);
    }
}

void ProductTrie::collect(TrieNode* node, const std::string& current,
                          std::vector<std::string>& results, size_t limit) {
    if (!node || results.size() >= limit) return;

    if (node->isWord) results.push_back(current);

    for (int i = 0; i < 26 && results.size() < limit; ++i) {
        if (node->children[i]) {
            collect(node->children[i], current + static_cast<char>('A' + i), results, limit);
        }
    }
}

std::vector<std::string> ProductTrie::suggest(const std::string& prefix, size_t limit) {
    std::string cleaned = normalize(prefix);
    TrieNode* node = root;

    for (char c : cleaned) {
        int idx = c - 'A';
        if (idx < 0 || idx >= 26) return {};
        if (!node->children[idx]) return {};
        node = node->children[idx];
    }

    std::vector<std::string> results;
    collect(node, cleaned, results, limit);
    return results;
}

bool ProductTrie::search(const std::string& word) {
    std::string cleaned = normalize(word);
    if (cleaned.empty()) return false;

    TrieNode* node = root;
    for (char c : cleaned) {
        int idx = c - 'A';
        if (idx < 0 || idx >= 26) return false;
        if (!node->children[idx]) return false;
        node = node->children[idx];
    }
    return node->isWord;
}

bool ProductTrie::eraseHelper(TrieNode* node, const std::string& word, int depth) {
    if (!node) return false;

    if (depth == static_cast<int>(word.size())) {
        if (!node->isWord) return false;
        node->isWord = false;

        for (auto child : node->children) {
            if (child) return false;
        }
        return true;
    }

    int idx = word[depth] - 'A';
    if (idx < 0 || idx >= 26) return false;

    if (eraseHelper(node->children[idx], word, depth + 1)) {
        delete node->children[idx];
        node->children[idx] = nullptr;

        if (node->isWord) return false;
        for (auto child : node->children) {
            if (child) return false;
        }
        return true;
    }
    return false;
}

void ProductTrie::erase(const std::string& word) {
    std::string cleaned = normalize(word);
    if (cleaned.empty()) return;
    eraseHelper(root, cleaned, 0);
}