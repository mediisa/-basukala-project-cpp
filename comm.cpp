#include "comm.h"
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <vector>
#include <iostream>

std::vector<User>    users;
//std::vector<Product> products;
ProductBST           productCatalog;
std::vector<Order>   orders;
std::vector<Package> packages;

namespace Common {

const double SHIPPING_COST = 100.0;

const std::string USERS_FILE = "users.txt";
const std::string PRODUCTS_FILE = "products.txt";
const std::string ORDERS_FILE = "orders.txt";
const std::string PACKAGES_FILE = "packages.txt";

const std::vector<std::string> PRODUCT_CATEGORIES = {
    "BOOK", "FOOD AND JUICE", "CLOTHES AND APPAREL", "ELECTRONICS", "OTHER"
};

static int orderSequence = 0;
static int packageSequence = 0;

std::string promptLine(const std::string& prompt) {
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    return input;
}

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
/*/
int getOrderSequence() {
    return orderSequence;
}
*/
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
        if (u.getusername() == username) {
            return &u;
        }
    }
    return nullptr;
}

Product* findProduct(const std::string& name) {
    return productCatalog.findGlobal(name);
}

Order* findOrder(const std::string& orderId) {
    for (auto& o : orders) {
        if (o.getid() == orderId) {
            return &o;
        }
    }
    return nullptr;
}

} 

/*bool PackageLess(const Package* a, const Package* b) {
    if (a->getscore() != b->getscore()) return a->getscore() < b->getscore(); 
    return a->getenqueueIndex() > b->getenqueueIndex();
}*/

bool PackageComparator::operator()(Package* a, Package* b) const {
    if (a->getscore() != b->getscore()) return a->getscore() < b->getscore(); 
    return a->getenqueueIndex() > b->getenqueueIndex(); 
}