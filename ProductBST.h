#ifndef PRODUCT_BST_H
#define PRODUCT_BST_H

#include <string>
#include <vector>
#include <array>
#include <functional>
#include "Models.h"

class ProductBST {
private:
    struct Node {
        Product data;
        Node* left;
        Node* right;
        Node(const Product& p) : data(p), left(nullptr), right(nullptr) {}
    };

    std::array<Node*, 5> roots;

    static int categoryIndex(const std::string& category);

    static bool lessByName(const Product& a, const Product& b);

    Node* insertNode(Node* root, const Product& p, bool& inserted);
    Node* findMin(Node* root);
    Node* removeNode(Node* root, const std::string& nameLower, bool& removed);

    Product* findNode(Node* root, const std::string& nameLower);
    void inOrder(Node* root, std::vector<Product>& out) const;
    void clear(Node* root);

public:
    ProductBST();
    ~ProductBST();

    bool insert(const Product& p);
    bool remove(const std::string& category, const std::string& name);
    Product* find(const std::string& category, const std::string& name);
    Product* findGlobal(const std::string& name);

    std::vector<Product> getCategoryProducts(const std::string& category) const;
    std::vector<Product> toVector() const;
    
    void clearAll();
};

#endif
