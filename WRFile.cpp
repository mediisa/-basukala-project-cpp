#include "WRFile.h"
#include "ProductTrie.h"
#include "User.h"
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <sstream>

WRFileService wrFileService;
long long globalPackageSequence = 0;

void WRFileService::ensureDefaultAdmin() {
    bool hasAdmin = false;
    for (const auto& u : users) {
        if (Common::equalsIgnoreCase(u.getusername(), "admin") && u.getrole() == "ADMIN") {
            hasAdmin = true;
            break;
        }
    }
    if (!hasAdmin) {
        UserService us;
        std::string salt = us.generateSalt(8);
        std::string hash = us.simpleHash(std::string("admin1220") + salt);
        User admin("Admin" , salt , "admin1220" , hash , "ADMIN");
        users.push_back(admin);
    }
}

int WRFileService::extractNumericSuffix(const std::string& id) {
    size_t pos = id.find_first_of("0123456789");
    if (pos == std::string::npos) return 0;
    std::string numeric = id.substr(pos);
    try {
        return std::stoi(numeric);
    } catch (...) {
        return 0;
    }
}

std::string WRFileService::encodeOrderItems(const std::vector<OrderItem>& items) {
    std::vector<std::string> segments;
    for (const auto& item : items) {
        std::stringstream ss;
        ss << item.productName << "@"
           << item.quantity << "@"
           << std::fixed << std::setprecision(2) << item.unitPrice;
        segments.push_back(ss.str());
    }
    return Common::join(segments, ';');
}

std::vector<OrderItem> WRFileService::decodeOrderItems(const std::string& encoded) {
    std::vector<OrderItem> items;
    if (encoded.empty()) return items;

    std::vector<std::string> tokens = Common::split(encoded, ';');
    for (const auto& token : tokens) {
        std::vector<std::string> parts = Common::split(token, '@');
        if (parts.size() < 3) continue;

        OrderItem item;
        item.productName = parts[0];
        item.quantity = std::stoi(parts[1]);
        item.unitPrice = std::stod(parts[2]);
        items.push_back(item);
    }
    return items;
}

void WRFileService::loadUsersFromFile() {
    users.clear();

    std::ifstream fin(Common::USERS_FILE);
    if (!fin.is_open()) {
        ensureDefaultAdmin();
        return;
    }

    std::string line;
    while (std::getline(fin, line)) {
        line = Common::trim(line);
        if (line.empty()) continue;

        std::vector<std::string> parts = Common::split(line, '|');
        if (parts.size() < 7) continue;

        User user(parts[0]  , parts[1] , parts[2] ,parts[3] , parts[4]);

        double balance = 0.0;
        int score = 0;      
        try {
            balance = std::stod(parts[5]);
        } catch (...) {
            balance = 0.0;
        }

        try {
            score = std::stoi(parts[6]);
        } catch (...) {
            score = 0;
        }

        user.addBlance(balance);
        user.addScore(score);
        users.push_back(user);

    }

    fin.close();
}

void WRFileService::saveUsersToFile() {
    std::ofstream fout(Common::USERS_FILE, std::ios::trunc);
    for (size_t i = 0; i < users.size(); ++i) {
        const auto& u = users[i];

        fout << u.getusername() << "|"
             << u.getsalt() << "|"
             << u.getpassword() << "|"
             << u.getpasswordHash() << "|"
             << u.getrole() << "|"
             << std::fixed << std::setprecision(2) << u.getbalance() << "|"
             << u.getscore() << "|";

        if (i + 1 < users.size()) fout << "\n";
    }
    fout.close();
}

void WRFileService::loadProductsFromFile() {
    productCatalog.toVector().clear();

    std::ifstream fin(Common::PRODUCTS_FILE);
    if (!fin.is_open()) return;

    std::string line , category = "OTHER";
    int stock , soldCount;
    double price;
    while (std::getline(fin, line)) {
        line = Common::trim(line);
        if (line.empty()) continue;

        std::vector<std::string> parts = Common::split(line, '|');
        if (parts.size() < 4) continue;

       
        std::string name = parts[0];

        try {
            price = std::stod(parts[1]);
        } catch (...) {
            price = 0.0;
        }

        try {
            stock = std::stoi(parts[2]);
        } catch (...) {
            stock = 0;
        }

        try {
            soldCount = std::stoi(parts[3]);
        } catch (...) {
            soldCount = 0;
        }

        if (parts.size() > 4) {
            std::string catCandidate = Common::trim(parts[4]);
            if (!catCandidate.empty()) {
                category = catCandidate;
            }
        }

        Product p(name,category,price,stock);
        p.addSold(soldCount);
        productCatalog.insert(p);
    }

    fin.close();

    productTrie.build(productCatalog.toVector());
}

void WRFileService::saveProductsToFile() {
    std::ofstream fout(Common::PRODUCTS_FILE, std::ios::trunc);
    auto list = productCatalog.toVector();
    for (size_t i = 0; i < list.size(); ++i) {
        const auto& p = list[i];

        fout << p.getname() << "|"
             << std::fixed << std::setprecision(2) << p.getprice() << "|"
             << p.getstock() << "|"
             << p.getsoldCount() << "|"
             << p.getcategory();

        if (i + 1 < list.size()) fout << "\n";
    }
    fout.close();
}

void WRFileService::loadOrdersFromFile() {
    orders.clear();
    int maxSeq = 0;

    std::ifstream fin(Common::ORDERS_FILE);
    if (!fin.is_open()) return;

    std::string line;
    while (std::getline(fin, line)) {
        line = Common::trim(line);
        if (line.empty()) continue;

        std::vector<std::string> parts = Common::split(line, '|');
        if (parts.size() < 7) continue;

        Order order(parts[0] , parts[1] , parts[2] , decodeOrderItems(parts[3]) , parts[5].empty() ? 'A' : parts[5][0] , parts[6] , parts[7] );

        double totalCost = 0.0;
        try {
            totalCost = std::stod(parts[4]);
        } catch (...) {
            totalCost = 0.0;
        }

        
        order.settotalCost(totalCost);
        orders.push_back(order);
        
        int seq = extractNumericSuffix(order.getid());
        if (seq > maxSeq) maxSeq = seq;
    }

    fin.close();
    Common::setOrderSequence(maxSeq);
}

void WRFileService::saveOrdersToFile() {
    std::ofstream fout(Common::ORDERS_FILE, std::ios::trunc);
    for (size_t i = 0; i < orders.size(); ++i) {
        const auto& o = orders[i];

        fout << o.getid() << "|"
             << o.getusername() << "|"
             << o.getpassworHash() << "|"
             << encodeOrderItems(o.getitems()) << "|"
             << std::fixed << std::setprecision(2) << o.gettotalCost() << "|"
             << o.getdestinationCity() << "|"
             << o.getaddress() << "|"
             << o.getstatus();

        if (i + 1 < orders.size()) fout << "\n";
    }
    fout.close();
}

void WRFileService::loadPackagesFromFile() {
    packages.clear();
    int maxSeq = 0;

    std::ifstream fin(Common::PACKAGES_FILE);
    if (!fin.is_open()) return;

    std::string line;
    while (std::getline(fin, line)) {
        line = Common::trim(line);
        if (line.empty()) continue;

        std::vector<std::string> parts = Common::split(line, '|');
        if (parts.size() < 11) continue;

        Package pack(parts[0] , parts[1] , parts[2] , parts[3] , decodeOrderItems(parts[4]) , parts[5].empty() ? 'A' : parts[5][0] , parts[6] , parts[8] , parts[9]);

    int score;
    int routeDistance;
    long long enqueueIndex;

        try {
            score = std::stoi(parts[7]);
        } catch (...) {
            score = 0;
        }

        try {
            routeDistance = std::stoi(parts[10]);
        } catch (...) {
            routeDistance = 0;
        }

        try {
            enqueueIndex = std::stoll(parts[11]);
        } catch (...) {
            enqueueIndex = 0;
        }

        pack.setscore(score);
        pack.setrouteDistance(routeDistance);
        pack.setenqueueIndex(enqueueIndex);
        packages.push_back(pack);

        int seq = extractNumericSuffix(pack.getid());
        if (seq > maxSeq) maxSeq = seq;
        if (pack.getenqueueIndex() > globalPackageSequence) {
            globalPackageSequence = pack.getenqueueIndex();
        }
    }

    fin.close();
    Common::setPackageSequence(maxSeq);
}

void WRFileService::savePackagesToFile() {
    std::ofstream fout(Common::PACKAGES_FILE, std::ios::trunc);
    for (size_t i = 0; i < packages.size(); ++i) {
        const auto& p = packages[i];

        fout << p.getid() << "|"
             << p.getorderId() << "|"
             << p.getusername() << "|"
             << p.getpasswordHash()<<"|"
             << encodeOrderItems(p.getitems()) << "|"
             << p.getdestinationCity() << "|"
             << p.getaddress() << "|"
             << p.getscore() << "|"
             << p.getstatus() << "|"
             << p.getrouteDisplay() << "|"
             << p.getrouteDistance() << "|"
             << p.getenqueueIndex();

        if (i + 1 < packages.size()) fout << "\n";
    }
    fout.close();
}

void WRFileService::loadAll() {
    loadUsersFromFile();
    loadProductsFromFile();
    loadOrdersFromFile();
    loadPackagesFromFile();
}

void WRFileService::saveAll() {
    saveUsersToFile();
    saveProductsToFile();
    saveOrdersToFile();
    savePackagesToFile();
}
