#include "common.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <utility>
#include <limits>


std::vector<User> users;
std::vector<Product> products;
std::vector<OrderRecord> orders;
std::vector<Package> packages;
ProductTrie productTrie;

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
    {'A', "Bimsy"},
    {'B', "Garous"},
    {'C', "Debrecen"},
    {'D', "Lae"},
    {'E', "Merribell"},
    {'F', "Wimbie"},
    {'G', "Aguascalientes"},
    {'H', "Froli"},
    {'I', "Tainan"},
    {'J', "Honara"},
    {'K', "Twinklehollow"},
    {'L', "Trinkleby"}
};

const std::vector<std::string> PRODUCT_CATEGORIES = {
    "Book",
    "Food",
    "تجهیزات پزشکی",
    "تجهیزات نقلیه",
    "دستگاه دیجیتال"
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
    addEdge('C', 'A', 5);
    addEdge('C', 'E', 4);
    addEdge('C', 'K', 4);
    addEdge('C', 'L', 3);
    addEdge('C', 'J', 6);
    addEdge('D', 'E', 1);
    addEdge('E', 'F', 2);
    addEdge('E', 'G', 3);
    addEdge('E', 'C', 4);
    addEdge('E', 'D', 1);
    addEdge('F', 'H', 3);
    addEdge('F', 'G', 5);
    addEdge('F', 'B', 5);
    addEdge('F', 'E', 2);
    addEdge('G', 'E', 3);
    addEdge('G', 'H', 4);
    addEdge('G', 'F', 5);
    addEdge('G', 'I', 2);
    addEdge('G', 'J', 4);
    addEdge('G', 'K', 10);
    addEdge('H', 'F', 3);
    addEdge('H', 'G', 4);
    addEdge('I', 'G', 2);
    addEdge('J', 'C', 6);
    addEdge('J', 'G', 4);
    addEdge('J', 'K', 8);
    addEdge('K', 'C', 4);
    addEdge('K', 'J', 8);
    addEdge('L', 'C', 3);

    return graph;
}

std::vector<std::string> split(const std::string &line, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(line);
    while (getline(ss, token, delimiter)) tokens.push_back(token);
    return tokens;
}

std::string join(const std::vector<std::string> &items, char delimiter) {
    std::string result;
    for (size_t i = 0; i < items.size(); ++i) {
        result += items[i];
        if (i + 1 < items.size()) result += delimiter;
    }
    return result;
}

std::string trim(const std::string &s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::string encodeOrderItems(const std::vector<OrderItem> &items) {
    std::vector<std::string> segments;
    for (const auto &item : items) {
        std::stringstream ss;
        ss << item.productName << "@"
           << item.quantity << "@"
           << std::fixed << std::setprecision(2) << item.unitPrice;
        segments.push_back(ss.str());
    }
    return join(segments, ';');
}

std::vector<OrderItem> decodeOrderItems(const std::string &encoded) {
    std::vector<OrderItem> items;
    if (encoded.empty()) return items;
    std::vector<std::string> tokens = split(encoded, ';');
    for (const auto &token : tokens) {
        std::vector<std::string> parts = split(token, '@');
        if (parts.size() < 3) continue;
        OrderItem item;
        item.productName = parts[0];
        item.quantity = stoi(parts[1]);
        item.unitPrice = stod(parts[2]);
        items.push_back(item);
    }
    return items;
}


User* findUser(const std::string &username) {
    for (auto &u : users) if (u.username == username) return &u;
    return nullptr;
}

Product* findProduct(const std::string &name) {
    for (auto &p : products) if (p.name == name) return &p;
    return nullptr;
}

OrderRecord* findOrder(const std::string &orderId) {
    for (auto &o : orders) if (o.id == orderId) return &o;
    return nullptr;
}

PathResult dijkstra(char start, char target,
                    const std::unordered_map<char, std::vector<std::pair<char, int>>> &graph) {
    std::unordered_map<char, int> dist;
    std::unordered_map<char, char> parent;
    for (const auto &entry : graph) dist[entry.first] = INT_MAX;
    dist[start] = 0;

    using PQItem = std::pair<int, char>;
    std::priority_queue<PQItem, std::vector<PQItem>, std::greater<PQItem>> pq;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [currentDist, node] = pq.top();
        pq.pop();
        if (currentDist != dist[node]) continue;
        if (node == target) break;
        for (auto &[neighbor, weight] : graph.at(node)) {
            if (dist[node] + weight < dist[neighbor]) {
                dist[neighbor] = dist[node] + weight;
                parent[neighbor] = node;
                pq.push({dist[neighbor], neighbor});
            }
        }
    }

    PathResult result;
    if (dist.find(target) == dist.end() || dist.at(target) == INT_MAX) return result;
    result.reachable = true;
    result.distance = dist[target];

    std::vector<char> reversed;
    char cur = target;
    reversed.push_back(cur);
    while (cur != start) {
        cur = parent[cur];
        reversed.push_back(cur);
    }
    reverse(reversed.begin(), reversed.end());
    result.path = reversed;
    return result;
}

PathResult shortestRouteFromNearestWarehouse(char destination) {
    auto graph = buildCityGraph();
    PathResult best;
    for (char warehouse : WAREHOUSE_CITIES) {
        PathResult candidate = dijkstra(warehouse, destination, graph);
        if (!candidate.reachable) continue;
        if (!best.reachable || candidate.distance < best.distance) {
            best = candidate;
        }
    }
    return best;
}

std::string formatRouteDisplay(const std::vector<char> &path, int distance) {
    if (path.empty()) return "Route unavailable";
    std::stringstream ss;
    for (size_t i = 0; i < path.size(); ++i) {
        ss << path[i];
        if (i + 1 < path.size()) ss << " -> ";
    }
    ss << " | Distance: " << distance;
    return ss.str();
}


void refreshUserHistoryStatus(User &user) {
    for (auto &entry : user.orderHistory) {
        size_t statusPos = entry.find("Status:");
        if (statusPos == std::string::npos) continue;
        size_t lb = entry.find('[');
        size_t rb = entry.find(']');
        if (lb == std::string::npos || rb == std::string::npos || rb <= lb + 1) continue;
        std::string orderId = entry.substr(lb + 1, rb - lb - 1);
        OrderRecord* order = findOrder(orderId);
        if (!order) continue;
        entry = entry.substr(0, statusPos) + "Status: " + order->status;
    }
}


bool PackageLess(const Package* a, const Package* b) {
    if (a->score != b->score) return a->score < b->score; 
    return a->enqueueIndex > b->enqueueIndex;
}

bool PackageComparator::operator()(Package* a, Package* b) const {
    if (a->score != b->score) return a->score < b->score; 
    return a->enqueueIndex > b->enqueueIndex; 
}

std::string toUpperCopy(const std::string &input) {
    std::string result;
    result.reserve(input.size());
    for (char ch : input) {
        result.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(ch))));
    }
    return result;
}

bool equalsIgnoreCase(const std::string &a, const std::string &b) {
    return toUpperCopy(a) == toUpperCopy(b);
}


bool resolveCategoryInput(const std::string &input, std::string &resolvedCategory) {
    std::string candidate = trim(input);
    if (candidate.empty()) return false;

    bool numeric = std::all_of(candidate.begin(), candidate.end(), [](unsigned char ch) {
        return std::isdigit(ch);
    });

    if (numeric) {
        try {
            size_t idx = std::stoul(candidate);
            if (idx >= 1 && idx <= PRODUCT_CATEGORIES.size()) {
                resolvedCategory = PRODUCT_CATEGORIES[idx - 1];
                return true;
            }
        } catch (...) {
            // fall through to string matching
        }
    }
    for (const auto &category : PRODUCT_CATEGORIES) {
        if (equalsIgnoreCase(category, candidate)) {
            resolvedCategory = category;
            return true;
        }
    }

    return false;
}