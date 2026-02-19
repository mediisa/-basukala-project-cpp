#include "comm.h"
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <vector>

std::vector<User>    users;
std::vector<Product> products;
std::vector<Order>   orders;
std::vector<Package> packages;

namespace Common {

const double SHIPPING_COST = 100.0;

const std::string USERS_FILE = "users.txt";
const std::string PRODUCTS_FILE = "products.txt";
const std::string ORDERS_FILE = "orders.txt";
const std::string PACKAGES_FILE = "packages.txt";

const std::vector<std::string> PRODUCT_CATEGORIES = {
    "Book",
    "Food",
    "juice",
    "Medical Equipment",
    "Electronics"
};

static int orderSequence = 0;
static int packageSequence = 0;

std::vector<std::string> split(const std::string& line, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(line);
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string join(const std::vector<std::string>& items, char delimiter) {
    std::string result;
    for (size_t i = 0; i < items.size(); ++i) {
        result += items[i];
        if (i + 1 < items.size()) result += delimiter;
    }
    return result;
}

std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::string encodeOrderItems(const std::vector<OrderItem>& items) {
    std::vector<std::string> segments;
    for (const auto& item : items) {
        std::stringstream ss;
        ss << item.productName << "@"
           << item.quantity << "@"
           << std::fixed << std::setprecision(2) << item.unitPrice;
        segments.push_back(ss.str());
    }
    return join(segments, ';');
}

std::vector<OrderItem> decodeOrderItems(const std::string& encoded) {
    std::vector<OrderItem> items;
    if (encoded.empty()) return items;

    std::vector<std::string> tokens = split(encoded, ';');
    for (const auto& token : tokens) {
        std::vector<std::string> parts = split(token, '@');
        if (parts.size() < 3) continue;

        OrderItem item;
        item.productName = parts[0];
        item.quantity = std::stoi(parts[1]);
        item.unitPrice = std::stod(parts[2]);
        items.push_back(item);
    }
    return items;
}

std::string toUpperCopy(const std::string& input) {
    std::string result;
    result.reserve(input.size());
    for (unsigned char ch : input) {
        result.push_back(static_cast<char>(std::toupper(ch)));
    }
    return result;
}

bool equalsIgnoreCase(const std::string& a, const std::string& b) {
    return toUpperCopy(a) == toUpperCopy(b);
}

bool resolveCategoryInput(const std::string& input, std::string& resolvedCategory) {
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
            // fall through
        }
    }

    for (const auto& category : PRODUCT_CATEGORIES) {
        if (equalsIgnoreCase(category, candidate)) {
            resolvedCategory = category;
            return true;
        }
    }

    return false;
}

void setOrderSequence(int value) {
    orderSequence = value;
}

void setPackageSequence(int value) {
    packageSequence = value;
}

int getOrderSequence() {
    return orderSequence;
}

int getPackageSequence() {
    return packageSequence;
}

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


User* findUser(const std::string& username) {
    for (auto& u : users) {
        if (u.username == username) {
            return &u;
        }
    }
    return nullptr;
}

Product* findProduct(const std::string& name) {
    for (auto& p : products) {
        if (equalsIgnoreCase(p.name, name)) {
            return &p;
        }
    }
    return nullptr;
}

Order* findOrder(const std::string& orderId) {
    for (auto& o : orders) {
        if (o.id == orderId) {
            return &o;
        }
    }
    return nullptr;
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

