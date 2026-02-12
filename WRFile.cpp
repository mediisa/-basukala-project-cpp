#include "WRFile.h"
#include "common.h"
#include <iostream>
#include <fstream>
#include <iomanip>


void loadUsersFromFile() {
    users.clear();
    std::ifstream fin(USERS_FILE);
    if (!fin.is_open()) {
        User admin;
        admin.username = "admin";
        admin.password = "admin123";
        admin.role = "ADMIN";
        admin.balance = 0.0;
        admin.score = 0;
        users.push_back(admin);
        return;
    }
    std::string line;
    while (getline(fin, line)) {
        line = trim(line);
        if (line.empty()) continue;
        std::vector<std::string> parts = split(line, '|');
        if (parts.size() < 6) continue;
        User user;
        user.username = parts[0];
        user.password = parts[1];
        user.role = parts[2];
        user.balance = stod(parts[3]);
        user.score = stoi(parts[4]);
        std::vector<std::string> historyItems = split(parts[5], ';');
        for (const auto &entry : historyItems) {
            if (!entry.empty()) user.orderHistory.push_back(entry);
        }
        users.push_back(user);
    }
    fin.close();
}

void saveUsersToFile() {
    std::ofstream fout(USERS_FILE, std::ios::trunc);
    for (size_t i = 0; i < users.size(); ++i) {
        const auto &u = users[i];
        std::string histEncoded = join(u.orderHistory, ';');
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

void loadProductsFromFile() {
    products.clear();
    std::ifstream fin(PRODUCTS_FILE);
    if (!fin.is_open()) {
        return;
    }
    std::string line;
    while (getline(fin, line)) {
        line = trim(line);
        if (line.empty()) continue;
        std::vector<std::string> parts = split(line, '|');
        if (parts.size() < 4) continue;
        Product p;
        p.name = parts[0];
        p.price = stod(parts[1]);
        p.stock = stoi(parts[2]);
        p.soldCount = stoi(parts[3]);
        products.push_back(p);
    }
        fin.close();
}

void saveProductsToFile() {
    std::ofstream fout(PRODUCTS_FILE, std::ios::trunc);
    for (size_t i = 0; i < products.size(); ++i) {
        const auto &p = products[i];
        fout << p.name << "|"
             << std::fixed << std::setprecision(2) << p.price << "|"
             << p.stock << "|"
             << p.soldCount;
        if (i + 1 < products.size()) fout << "\n";
    }
    fout.close();
}

void loadOrdersFromFile() {
    orders.clear();
    std::ifstream fin(ORDERS_FILE);
    if (!fin.is_open()) return;
    std::string line;
    while (getline(fin, line)) {
        line = trim(line);
        if (line.empty()) continue;
        std::vector<std::string> parts = split(line, '|');
        if (parts.size() < 7) continue;
        OrderRecord order;
        order.id = parts[0];
        order.username = parts[1];
        order.items = decodeOrderItems(parts[2]);
        order.totalCost = stod(parts[3]);
        order.destinationCity = parts[4][0];
        order.address = parts[5];
        order.status = parts[6];
        orders.push_back(order);

        std::string numeric = order.id.substr(order.id.find_first_of("0123456789"));
        if (!numeric.empty()) {
            orderSequence = std::max(orderSequence, stoi(numeric));
        }
    }
    fin.close();
}

void saveOrdersToFile() {
    std::ofstream fout(ORDERS_FILE, std::ios::trunc);
    for (size_t i = 0; i < orders.size(); ++i) {
        const auto &o = orders[i];
        fout << o.id << "|"
             << o.username << "|"
             << encodeOrderItems(o.items) << "|"
             << std::fixed << std::setprecision(2) << o.totalCost << "|"
             << o.destinationCity << "|"
             << o.address << "|"
             << o.status;
        if (i + 1 < orders.size()) fout << "\n";
    }
    fout.close();
}

void loadPackagesFromFile() {
    packages.clear();
    std::ifstream fin(PACKAGES_FILE);
    if (!fin.is_open()) return;
    std::string line;
    while (getline(fin, line)) {
        line = trim(line);
        if (line.empty()) continue;
        std::vector<std::string> parts = split(line, '|');
        if (parts.size() < 11) continue;
        Package pack;
        pack.id = parts[0];
        pack.orderId = parts[1];
        pack.username = parts[2];
        pack.items = decodeOrderItems(parts[3]);
        pack.destinationCity = parts[4][0];
        pack.address = parts[5];
        pack.score = stoi(parts[6]);
        pack.status = parts[7];
        pack.routeDisplay = parts[8];
        pack.routeDistance = stoi(parts[9]);
        pack.enqueueIndex = stoll(parts[10]);
        packages.push_back(pack);

        std::string numeric = pack.id.substr(pack.id.find_first_of("0123456789"));
        if (!numeric.empty()) packageSequence = std::max(packageSequence, stoi(numeric));
        globalPackageSequence = std::max(globalPackageSequence, pack.enqueueIndex);
    }
    fin.close();
}

void savePackagesToFile() {
    std::ofstream fout(PACKAGES_FILE, std::ios::trunc);
    for (size_t i = 0; i < packages.size(); ++i) {
        const auto &p = packages[i];
        fout << p.id << "|"
             << p.orderId << "|"
             << p.username << "|"
             << encodeOrderItems(p.items) << "|"
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