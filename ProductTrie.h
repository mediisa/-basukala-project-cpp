#ifndef PRODUCTTRIE_H
#define PRODUCTTRIE_H

#include "Models.h"
#include "ProductBST.h"
#include <string>
#include <vector>

struct TrieNode {
    bool isWord;
    std::vector<TrieNode*> children;
    TrieNode();
};

class ProductTrie {
private:
    TrieNode* root;

    void clear(TrieNode* node);
    void collect(TrieNode* node, const std::string& current,
                 std::vector<std::string>& results, size_t limit);
    bool eraseHelper(TrieNode* node, const std::string& word, int depth);

    std::string normalize(const std::string& input) const;

public:
    ProductTrie();
    ~ProductTrie();

    void insert(const std::string& word);
    void build(const std::vector<Product>& products);
    ///void build(const ProductBST& catalog);

    std::vector<std::string> suggest(const std::string& prefix, size_t limit = 5);
    bool search(const std::string& word);
    void erase(const std::string& word);
};

extern ProductTrie productTrie;


#endif