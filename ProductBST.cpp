#include "ProductBST.h"
#include "comm.h"
#include <algorithm>

ProductBST::ProductBST() {
    roots.fill(nullptr);
}

ProductBST::~ProductBST() {
    clearAll();
}

int ProductBST::categoryIndex(const std::string& category) {
    static const std::array<std::string, 5> cats = {
        "BOOK", "FOOD AND JUICE", "CLOTHES AND APPAREL", "ELECTRONICS", "OTHER"
    };
    std::string c = Common::toUpperCopy(category);
    for (int i = 0; i < 5; ++i) {
        if (c == cats[i]) return i;
    }
    return 4;
}

bool ProductBST::lessByName(const Product& a, const Product& b) {
    std::string an = Common::toUpperCopy(a.getname());
    std::string bn = Common::toUpperCopy(b.getname());
    if (an != bn) return an < bn;
    return a.getname() < b.getname();
}

ProductBST::Node* ProductBST::insertNode(Node* root, const Product& p, bool& inserted) {
    if (!root) {
        inserted = true;
        return new Node(p);
    }
    if (lessByName(p, root->data)) {
        root->left = insertNode(root->left, p, inserted);
    } else if (lessByName(root->data, p)) {
        root->right = insertNode(root->right, p, inserted);
    } else {
        inserted = false;
    }
    return root;
}

ProductBST::Node* ProductBST::findMin(Node* root) {
    while (root && root->left) root = root->left;
    return root;
}

ProductBST::Node* ProductBST::removeNode(Node* root, const std::string& nameLower, bool& removed) {
    if (!root) return nullptr;

    std::string cur = Common::toUpperCopy(root->data.getname());
    if (nameLower < cur) {
        root->left = removeNode(root->left, nameLower, removed);
    } else if (nameLower > cur) {
        root->right = removeNode(root->right, nameLower, removed);
    } else {
        removed = true;
        if (!root->left) {
            Node* r = root->right;
            delete root;
            return r;
        }
        if (!root->right) {
            Node* l = root->left;
            delete root;
            return l;
        }
        Node* minNode = findMin(root->right);
        root->data = minNode->data;
        root->right = removeNode(root->right, Common::toUpperCopy(minNode->data.getname()), removed);
    }
    return root;
}

Product* ProductBST::findNode(Node* root, const std::string& nameLower) {
    if (!root) return nullptr;
    std::string cur = Common::toUpperCopy(root->data.getname());
    if (nameLower < cur) return findNode(root->left, nameLower);
    if (nameLower > cur) return findNode(root->right, nameLower);
    return &root->data;
}

void ProductBST::inOrder(Node* root, std::vector<Product>& out) const {
    if (!root) return;
    inOrder(root->left, out);
    out.push_back(root->data);
    inOrder(root->right, out);
}

void ProductBST::clear(Node* root) {
    if (!root) return;
    clear(root->left);
    clear(root->right);
    delete root;
}

bool ProductBST::insert(const Product& p) {
    int idx = categoryIndex(p.getcategory());
    bool inserted = false;
    roots[idx] = insertNode(roots[idx], p, inserted);
    return inserted;
}

bool ProductBST::remove(const std::string& category, const std::string& name) {
    int idx = categoryIndex(category);
    bool removed = false;
    roots[idx] = removeNode(roots[idx], Common::toUpperCopy(name), removed);
    return removed;
}

Product* ProductBST::find(const std::string& category, const std::string& name) {
    int idx = categoryIndex(category);
    return findNode(roots[idx], Common::toUpperCopy(name));
}

Product* ProductBST::findGlobal(const std::string& name) {
    std::string key = Common::toUpperCopy(name);
    for (int i = 0; i < 5; ++i) {
        Product* p = findNode(roots[i], key);
        if (p) return p;
    }
    return nullptr;
}

std::vector<Product> ProductBST::getCategoryProducts(const std::string& category) const {
    int idx = categoryIndex(category);
    std::vector<Product> out;
    inOrder(roots[idx], out);
    return out;
}

std::vector<Product> ProductBST::toVector() const {
    std::vector<Product> out;
    for (int i = 0; i < 5; ++i) {
        inOrder(roots[i], out);
    }
    return out;
}

void ProductBST::clearAll() {
    for (int i = 0; i < 5; ++i) {
        clear(roots[i]);
        roots[i] = nullptr;
    }
}
