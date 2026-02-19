#include "WRFile.h"
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <sstream>

WRFileService wrFileService;
long long globalPackageSequence = 0;

void WRFileService::ensureDefaultAdmin() {
    bool hasAdmin = false;
    for (const auto& u : users) {
        if (Common::equalsIgnoreCase(u.username, "admin") && u.role == "ADMIN") {
            hasAdmin = true;
            break;
        }
    }
    if (!hasAdmin) {
        User admin;
        admin.username = "admin";
        admin.password = "admin123";
        admin.role = "ADMIN";
        admin.balance = 0.0;
        admin.score = 0;
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

void WRFileService::loadUsersFromFile() {
    //users.clear();

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
        if (parts.size() < 5) continue;

        User user;
        user.username = parts[0];
        user.password = parts[1];
        user.role = parts[2];

        try {
            user.balance = std::stod(parts[3]);
        } catch (...) {
            user.balance = 0.0;
        }

        try {
            user.score = std::stoi(parts[4]);
        } catch (...) {
            user.score = 0;
        }

       users.push_back(user);
       if (parts.size() >= 6) {
            auto historyItems = Common::split(parts[5], ';');
            for (const auto& h : historyItems){
                if (!h.empty()){
                    user.orderHistory.push_back(h);
                }
            }    
        }
    }

    fin.close();
}

void WRFileService::saveUsersToFile() {
    std::ofstream fout(Common::USERS_FILE, std::ios::trunc);
    for (size_t i = 0; i < users.size(); ++i) {
        const auto& u = users[i];
        std::string histEncoded = Common::join(u.orderHistory, ';');

        fout << u.username << "|"
             << u.password << "|"
             << u.role << "|"
             << std::fixed << std::setprecision(2) << u.balance << "|"
             << u.score << "|"
             << histEncoded;

        if (i + 1 < users.size()) fout << "\n";
    }
    fout.close();
}

void WRFileService::loadProductsFromFile() {
    products.clear();

    std::ifstream fin(Common::PRODUCTS_FILE);
    if (!fin.is_open()) return;

    std::string line;
    while (std::getline(fin, line)) {
        line = Common::trim(line);
        if (line.empty()) continue;

        std::vector<std::string> parts = Common::split(line, '|');
        if (parts.size() < 4) continue;

        Product p;
        p.name = parts[0];

        try {
            p.price = std::stod(parts[1]);
        } catch (...) {
            p.price = 0.0;
        }

        try {
            p.stock = std::stoi(parts[2]);
        } catch (...) {
            p.stock = 0;
        }

        try {
            p.soldCount = std::stoi(parts[3]);
        } catch (...) {
            p.soldCount = 0;
        }


        if (!Common::PRODUCT_CATEGORIES.empty()) {
            p.category = Common::PRODUCT_CATEGORIES.front();
        } else {
            p.category = "Unspecified";
        }

        if (parts.size() > 4) {
            std::string catCandidate = Common::trim(parts[4]);
            if (!catCandidate.empty()) {
                bool matched = false;
                for (const auto& allowed : Common::PRODUCT_CATEGORIES) {
                    if (Common::equalsIgnoreCase(allowed, catCandidate)) {
                        p.category = allowed;
                        matched = true;
                        break;
                    }
                }
                if (!matched) {
                    p.category = catCandidate;
                }
            }
        }

    
        bool recognized = Common::PRODUCT_CATEGORIES.empty() ? true :
            std::any_of(Common::PRODUCT_CATEGORIES.begin(),
                        Common::PRODUCT_CATEGORIES.end(),
                        [&](const std::string& allowed) {
                            return Common::equalsIgnoreCase(allowed, p.category);
                        });

        if (!recognized && !Common::PRODUCT_CATEGORIES.empty()) {
            p.category = Common::PRODUCT_CATEGORIES.front();
        }

        products.push_back(p);
    }

    fin.close();
}

void WRFileService::saveProductsToFile() {
    std::ofstream fout(Common::PRODUCTS_FILE, std::ios::trunc);
    for (size_t i = 0; i < products.size(); ++i) {
        const auto& p = products[i];

        fout << p.name << "|"
             << std::fixed << std::setprecision(2) << p.price << "|"
             << p.stock << "|"
             << p.soldCount << "|"
             << p.category;

        if (i + 1 < products.size()) fout << "\n";
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

        Order order;
        order.id = parts[0];
        order.username = parts[1];
        order.items = Common::decodeOrderItems(parts[2]);

        try {
            order.totalCost = std::stod(parts[3]);
        } catch (...) {
            order.totalCost = 0.0;
        }

        order.destinationCity = parts[4].empty() ? 'A' : parts[4][0];
        order.address = parts[5];
        order.status = parts[6];

        orders.push_back(order);

        int seq = extractNumericSuffix(order.id);
        if (seq > maxSeq) maxSeq = seq;
    }

    fin.close();
    Common::setOrderSequence(maxSeq);
}

void WRFileService::saveOrdersToFile() {
    std::ofstream fout(Common::ORDERS_FILE, std::ios::trunc);
    for (size_t i = 0; i < orders.size(); ++i) {
        const auto& o = orders[i];

        fout << o.id << "|"
             << o.username << "|"
             << Common::encodeOrderItems(o.items) << "|"
             << std::fixed << std::setprecision(2) << o.totalCost << "|"
             << o.destinationCity << "|"
             << o.address << "|"
             << o.status;

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

        Package pack;
        pack.id = parts[0];
        pack.orderId = parts[1];
        pack.username = parts[2];
        pack.items = Common::decodeOrderItems(parts[3]);
        pack.destinationCity = parts[4].empty() ? 'A' : parts[4][0];
        pack.address = parts[5];

        try {
            pack.score = std::stoi(parts[6]);
        } catch (...) {
            pack.score = 0;
        }

        pack.status = parts[7];
        pack.routeDisplay = parts[8];

        try {
            pack.routeDistance = std::stoi(parts[9]);
        } catch (...) {
            pack.routeDistance = 0;
        }

        try {
            pack.enqueueIndex = std::stoll(parts[10]);
        } catch (...) {
            pack.enqueueIndex = 0;
        }

        packages.push_back(pack);

        int seq = extractNumericSuffix(pack.id);
        if (seq > maxSeq) maxSeq = seq;
        if (pack.enqueueIndex > globalPackageSequence) {
            globalPackageSequence = pack.enqueueIndex;
        }
    }

    fin.close();
    Common::setPackageSequence(maxSeq);
}

void WRFileService::savePackagesToFile() {
    std::ofstream fout(Common::PACKAGES_FILE, std::ios::trunc);
    for (size_t i = 0; i < packages.size(); ++i) {
        const auto& p = packages[i];

        fout << p.id << "|"
             << p.orderId << "|"
             << p.username << "|"
             << Common::encodeOrderItems(p.items) << "|"
             << p.destinationCity << "|"
             << p.address << "|"
             << p.score << "|"
             << p.status << "|"
             << p.routeDisplay << "|"
             << p.routeDistance << "|"
             << p.enqueueIndex;

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
