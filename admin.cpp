#include "admin.h"
#include "WRFile.h"
#include "common.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <queue>

void displayProducts(){
    std::cout << "\n--- Product Catalog ---\n";
    if (products.empty()) {
        std::cout << "No products available.\n";
        return;
    }
    for (size_t i = 0; i < products.size(); ++i) {
        const auto &p = products[i];
        std::cout << i + 1 << ". " << p.name
             << " | Price: $" << std::fixed << std::setprecision(2) << p.price
             << " | Stock: " << p.stock
             << " | Sold: " << p.soldCount << "\n";
    }
}

void displayUsers(){
    std::cout << "\n--- Registered Users ---\n";
    for (const auto &u : users) {
        std::cout << "Username: " << u.username
             << " | Role: " << u.role
             << " | Balance: $" << std::fixed << std::setprecision(2) << u.balance
             << " | Score: " << u.score << "\n";
    }
}

void displayPendingPackages(){
    std::cout << "\n--- Pending Packages ---\n";
    std::priority_queue<Package*, std::vector<Package*>, PackageComparator> pq;
    for (auto &pack : packages)
        if (pack.status == "Pending") pq.push(&pack);

    if (pq.empty()) {
        std::cout << "No pending packages.\n";
        return;
    }

    int counter = 1;
    while (!pq.empty()) {
        Package* pack = pq.top();
        pq.pop();
        std::cout << counter++ << ". Package ID: " << pack->id
             << " | Order ID: " << pack->orderId
             << " | User: " << pack->username
             << " | Destination: " << pack->destinationCity << " (" << CITY_LABELS.at(pack->destinationCity) << ")"
             << " | Score: " << pack->score
             << " | Queue Position Index: " << pack->enqueueIndex << "\n";
    }
}

void addProduct(){
    std::cout << "\n--- Add Product ---\n";
    std::string name;
    std::cout << "Product name: ";
    std::getline(std::cin, name);
    if (name.empty()) {
        std::cout << "Name cannot be empty.\n";
        return;
    }
        if (findProduct(name)) {
        std::cout << "Product already exists.\n";
        return;
    }
    std::string priceInput, stockInput;
    std::cout << "Price (USD): ";
    std::getline(std::cin, priceInput);
    std::cout << "Stock quantity: ";
    std::getline(std::cin, stockInput);
    try {
        double price = stod(priceInput);
        int stock = stoi(stockInput);
        if (price < 0 || stock < 0) {
            std::cout << "Price and stock must be non-negative.\n";
            return;
        }
        Product p;
        p.name = name;
        p.price = price;
        p.stock = stock;
        p.soldCount = 0;
        products.push_back(p);
        productTrie.build(products);
        saveProductsToFile();
        std::cout << "Product added successfully.\n";
    } catch (...) {
        std::cout << "Invalid numeric input.\n";
    }
}

void deleteProduct(){
    std::cout << "\n--- Delete Product ---\n";
    std::string name;
    std::cout << "Product name to delete: ";
    std::getline(std::cin, name);
    for (auto it = products.begin(); it != products.end(); ++it) {
        if (it->name == name) {
            products.erase(it);
            productTrie.build(products);
            saveProductsToFile();
            std::cout << "Product removed.\n";
            return;
        }
    }
    std::cout << "Product not found.\n";
}

void processNextPackage(){
    std::priority_queue<Package*, std::vector<Package*>, PackageComparator> pq;
    for (auto &pack : packages) {
        if (pack.status == "Pending") pq.push(&pack);
    }
    if (pq.empty()) {
        std::cout << "No packages to process.\n";
        return;
    }
    Package* pack = pq.top();
    pq.pop();

    std::cout << "\n--- Processing Package ---\n";
    std::cout << "Package ID: " << pack->id << "\n";
    std::cout << "Order ID: " << pack->orderId << "\n";
    std::cout << "Customer: " << pack->username << "\n";
    std::cout << "Destination: " << pack->destinationCity << " (" << CITY_LABELS.at(pack->destinationCity) << ")\n";
    std::cout << "Address: " << pack->address << "\n";
    std::cout << "Package Score (total items): " << pack->score << "\n";
    std::cout << "Products:\n";
    for (const auto &item : pack->items) {
        std::cout << "  - " << item.productName << " x" << item.quantity << "\n";
    }  
    
    PathResult route = shortestRouteFromNearestWarehouse(pack->destinationCity);
    if (!route.reachable) {
        std::cout << "No route available from warehouses to destination.\n";
        return;
    }

    pack->routeDisplay = formatRouteDisplay(route.path, route.distance);
    pack->routeDistance = route.distance;
    pack->status = "Delivered";

    std::cout << "Computed Route: " << pack->routeDisplay << "\n";

    OrderRecord* order = findOrder(pack->orderId);
    if (order) order->status = "Delivered";

    User* user = findUser(pack->username);
    if (user) {
        user->score += pack->score;
        refreshUserHistoryStatus(*user);
    } 

    saveUsersToFile();
    saveOrdersToFile();
    savePackagesToFile();

    std::cout << "Package status updated to Delivered. User score increased.\n";  
}

void adminMenu(User &adminUser) {
    while (true) {
        std::cout << "\n=== Admin Menu ===\n"
             << "1. View Products\n"
             << "2. Add Product\n"
             << "3. Delete Product\n"
             << "4. View Users\n"
             << "5. View Pending Packages\n"
             << "6. Process Next Package\n"
             << "7. Logout\n"
             << "Select an option: ";
        std::string choice;
        std::getline(std::cin, choice);
        if (choice == "1") {
            displayProducts();
        } else if (choice == "2") {
            addProduct();
        } else if (choice == "3") {
            deleteProduct();
        } else if (choice == "4") {
            displayUsers();
        } else if (choice == "5") {
            displayPendingPackages();
        } else if (choice == "6") {
            processNextPackage();
        } else if (choice == "7") {
            std::cout << "Logging out...\n";
            break;
        } else {
            std::cout << "Invalid choice.\n";
        }
    }
}