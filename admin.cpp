#include "Admin.h"
#include "User.h"
#include "WRFile.h"
#include "comm.h"
#include "ProductTrie.h"
#include "Graph.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <queue>
#include <sstream>
#include <MAP>


AdminService::AdminService(UserService& userService)
    : userService_(userService) {}

std::string AdminService::promptLine(const std::string& prompt) {
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    return input;
}

bool AdminService::tryParseInt(const std::string& s, int& out) {
    try {
        size_t idx = 0;
        int v = std::stoi(s, &idx);
        if (idx != s.size()) return false;
        out = v;
        return true;
    } catch (...) {
        return false;
    }
}

void AdminService::displayProducts() {
        std::cout << "\n--- displayProducts ---"
                  << "\nSelect method:\n"
                  << "  1) Select by category list (number)\n"
                  << "  2) Select by Top Selling \n"
                  << "  3) Select by Name\n"
                  << "  4) Select by Price\n"
                  << "  5) Back\n"
                  << "Choose: ";
        std::string m;
        std::getline(std::cin, m);

        if( m == "1"){
            selectProductByCategoryByNumber();
        }else if( m == "2"){
            displayTopSellingProduct();
        }else if( m == "3"){
            displayByName();
        }else if( m == "4"){
            displayByPriceProduct();
        }else if( m == "5"){
            return;
        }else{
            std::cout << "Invalid option.\n ";
        }
}

void AdminService::printCategoryOptions() {
    std::cout << "Available categories:\n";
    for (size_t i = 0; i < Common::PRODUCT_CATEGORIES.size(); ++i) {
        std::cout << "  " << i + 1 << ". " << Common::PRODUCT_CATEGORIES[i] << "\n";
    }
}

void AdminService::displayProductsByCategorySorted(const std::string& categoryFilter) {
    std::vector<Product*> list;
    for (auto& p : products) {
        if (!categoryFilter.empty() && !Common::equalsIgnoreCase(p.category, categoryFilter)) continue;
        list.push_back(&p);
    }

    std::sort(list.begin(), list.end(), [](const Product* a, const Product* b) {
        return Common::toUpperCopy(a->name) < Common::toUpperCopy(b->name);
    });

    if (list.empty()) {
        if (!categoryFilter.empty()) std::cout << "No products found in the selected category.\n";
        else std::cout << "No products available.\n";
        return ;
    }

    for (size_t i = 0; i < list.size(); ++i) {
        const auto* product = list[i];
        std::cout << i  << ". " << product->name
                  << " | Category: " << (product->category.empty() ? "-" : product->category)
                  << " | Price: $" << std::fixed << std::setprecision(2) << product->price 
                  << " | Stock: " << product->stock 
                  << " | Sold: " << product->soldCount <<" \n";
    }

    return ;
}

void AdminService::selectProductByCategoryByNumber() {
    if (products.empty()) {
        std::cout << "No products available.\n";
        return ;
    }

    printCategoryOptions();
    std::string categoryInput = promptLine("Filter by category (number or name, Enter for all): ");
    std::string resolvedCategory;

    if (!categoryInput.empty() && !Common::resolveCategoryInput(categoryInput, resolvedCategory)) {
        std::cout << "Invalid category selection. Showing all categories.\n";
        resolvedCategory.clear();
    }

    displayProductsByCategorySorted(resolvedCategory);

}

void AdminService::displayTopSellingProduct(){
    if (products.empty()) {
        std::cout << "No products available.\n";
        return;
    }

    std::cout << "\n ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ \n";

    auto sorted = products;
    std::sort(sorted.begin() , sorted.end() , 
            [](const Product& a , const Product& b){
                return a.soldCount > b.soldCount;
            });


    for (size_t i=0 ; i < products.size() ; ++i){
        const auto& p = sorted[i];
        std::cout << i+1 << ". "<<p.name
                  << " | Price: $"<< std::fixed << std::setprecision(2) << p.price
                  << " | Category: "<<p.category 
                  << " | Stock: " << p.stock 
                  << " | Sold: " << p.soldCount <<" \n";
    }
}

void AdminService::displayByName(){
    if (products.empty()) {
        std::cout << "No products available.\n";
        return;
    }

    std::cout << "\n ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ \n";

    auto sorted = products;
    std::sort(sorted.begin() , sorted.end() , 
            [](const Product& a , const Product& b){
                return a.name < b.name;
            });


    for (size_t i=0 ; i < products.size() ; ++i){
        const auto& p = sorted[i];
        std::cout << i+1 << ". "<<p.name
                  << " | Price: $"<< std::fixed << std::setprecision(2) << p.price
                  << " | Category: "<<p.category 
                  << " | Stock: " << p.stock 
                  << " | Sold: " << p.soldCount <<" \n";
    }
}

void AdminService::displayByPriceProduct(){
    if (products.empty()) {
        std::cout << "No products available.\n";
        return;
    }

    std::cout << "\n ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ \n";

    auto sorted = products;
    std::sort(sorted.begin() , sorted.end() , 
            [](const Product& a , const Product& b){
                return a.price > b.price;
            });


    for (size_t i=0 ; i < products.size() ; ++i){
        const auto& p = sorted[i];
        std::cout << i+1 << ". "<<p.name
                  << " | Price: $"<< std::fixed << std::setprecision(2) << p.price
                  << " | Category: "<<p.category 
                  << " | Stock: " << p.stock 
                  << " | Sold: " << p.soldCount <<" \n";
    }
}

void AdminService::displayUsers() {
    std::cout << "\n--- Registered Users ---\n";

    auto sorted = users;
    std::sort(sorted.begin() , sorted.end() , 
            [](const User& a , const User& b){
                return a.username < b.username;
            });

    for (size_t i=0 ; i < users.size() ; ++i) {
        const auto& u = sorted[i];
        std::cout << "Username: " << u.username
                  << " | Role: " << u.role
                  << " | Balance: $" << std::fixed << std::setprecision(2) << u.balance
                  << " | Score: " << u.score << "\n";
    }
}

void AdminService::displayPendingPackages() {
    std::cout << "\n--- Pending Packages ---\n";
    std::priority_queue<Package*, std::vector<Package*>, PackageComparator> pq;
    for (auto& pack : packages) {
        if (pack.status == "Pending") pq.push(&pack);
    }

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
                  << " | Destination: " << pack->destinationCity
                  << " (" << CITY_LABELS.at(pack->destinationCity) << ")"
                  << " | Score: " << pack->score
                  << " | Queue Position Index: " << pack->enqueueIndex << "\n";
    }
}

void AdminService::addProduct() {
    std::cout << "\n--- Add Product ---\n";
    std::string countInput;
    std::cout << "How many items do you want to add ? \n";
    std::getline(std::cin, countInput);

    int cho = 0;
    try {
        cho = std::stoi(countInput);
    } catch (...) {
        std::cout << "Invalid number.\n";
        return;
    }

    if (cho <= 0) {
        std::cout << "Number must be positive.\n";
        return;
    }

    while (cho > 0) {
        std::string name = promptLine("Product name: ");

        if (name.empty()) {
            std::cout << "Name cannot be empty.\n";
            return;
        }

        if (Common::findProduct(name)) {
            std::cout << "Product already exists.\n";
            while(true){
                std::string input = promptLine("Do you want edit ?(Y/N) \n");
                if(Common::toUpperCopy(input) == "Y" ){ editProduct(); return;}
                else if(Common::toUpperCopy(input) == "N"){ return;}
                std::cout << "Please enter the Y/y or N/n.  ";
            }
        }

        std::string priceInput, stockInput;
        std::cout << "Price (USD): ";
        std::getline(std::cin, priceInput);
        std::cout << "Stock quantity: ";
        std::getline(std::cin, stockInput);

        try {
            double price = std::stod(priceInput);
            int stock = std::stoi(stockInput);
            if (price < 0 || stock < 0) {
                std::cout << "Price and stock must be non-negative.\n";
                return;
            }

            std::string categorySelection;
            std::string resolvedCategory;
            while (true) {
                printCategoryOptions();
                std::cout << "Select category (number or name): ";
                std::getline(std::cin, categorySelection);
                if (Common::resolveCategoryInput(categorySelection, resolvedCategory)) {
                    break;
                }
                std::cout << "Invalid category selection. Please try again.\n";
            }

            Product p;
            p.name = name;
            p.price = price;
            p.stock = stock;
            p.soldCount = 0;
            p.category = resolvedCategory;

            products.push_back(p);
            productTrie.build(products);
            wrFileService.saveProductsToFile();

            std::cout << "Product added successfully.\n";
        } catch (...) {
            std::cout << "Invalid numeric input.\n";
        }

        cho--;
    }
}

void AdminService::deleteProduct() {
    std::cout << "\n--- Delete Product ---\n";
    std::string name;
    std::cout << "Product name to delete: ";
    std::getline(std::cin, name);

    for (auto it = products.begin(); it != products.end(); ++it) {
        if (Common::equalsIgnoreCase(it->name , name)) {
            products.erase(it);
            productTrie.build(products);
            wrFileService.saveProductsToFile();
            std::cout << "Product removed.\n";
            return;
        }
    }
    std::cout << "Product not found.\n";
}

void AdminService::editProduct() {
    std::cout << "\n--- Edit Product ---\n";
    std::string name;
    std::cout << "Product name to edit: ";
    std::getline(std::cin, name);
   
    std::cout << "\n--- Modify Product ---\n"
              << "1) Edit price\n"
              << "2) Edit stock quantity\n"
              << "3) Edit name\n"
              << "4) Edit category\n"
              << "5) back\n"
              << "Choose: ";
    std::string m;
    std::getline(std::cin, m);
    for (auto it = products.begin(); it != products.end(); ++it) {
        if (Common::equalsIgnoreCase(it->name , name)) {
            if(m == "1"){
                std::string input = promptLine("New price: ");
                try {
                    double newPrice = std::stod(input);
                    if (newPrice < 0) {
                        std::cout << "Price cannot be negative.\n";
                        return;
                    }
                    it->setPrice(newPrice);
                    std::cout << "Price updated successfully.\n";
                    wrFileService.saveProductsToFile();
                    return;
                } catch (...) {
                    std::cout << "Invalid price.\n";
                    return;
                }
            }else if(m == "2"){
                std::string input = promptLine("Stock quantity change (+/-): ");
                int delta = 0;

                if(!tryParseInt(input , delta)) {std::cout << "Invalid stock value.\n";  return;}

                if(it->stock + delta < 0) {std::cout << "Stock cannot be negative.\n";  return;}

                if (delta > 0){
                    it->addStock(delta);
                }else{
                    it->reduceStock(-delta);
                }

                std::cout << "Stock quantity updated successfully.\n";
                wrFileService.saveProductsToFile();
                return;
            }else if(m == "3"){
                std::string newName = promptLine("New product name: ");
                if(newName.empty()){
                    std::cout << "Name cannot be empty.\n"; return;
                }

                it->setName(newName);
                std::cout << "Name updated successfully.\n";
                wrFileService.saveProductsToFile();
                return;
            }else if(m == "4"){
                printCategoryOptions();
                std::string input = promptLine("New category (number or name): ");
                std::string resolved;
                if(!Common::resolveCategoryInput(input , resolved)){ std::cout << "Invalid category.\n"; return;}

                it->setCategory(resolved);
                std::cout << "Categoyr updated successfully.\n";
                wrFileService.saveProductsToFile();
                return;
            }else if(m == "5"){
                return;
            }
        }
    }

    std::cout << "Product not found.\n";
    return;
}

void AdminService::processNextPackage() {
    std::priority_queue<Package*, std::vector<Package*>, PackageComparator> pq;
    for (auto& pack : packages) {
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
    std::cout << "Destination: " << pack->destinationCity
              << " (" << CITY_LABELS.at(pack->destinationCity) << ")\n";
    std::cout << "Address: " << pack->address << "\n";
    std::cout << "Package Score (total items): " << pack->score << "\n";
    std::cout << "Products:\n";

    for (const auto& item : pack->items) {
        Product* prod = Common::findProduct(item.productName);
        std::cout << "  - " << item.productName;
        if (prod && !prod->category.empty()) {
            std::cout << " [" << prod->category << "]";
        }
        std::cout << " x" << item.quantity << "\n";
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

    Order* order = Common::findOrder(pack->orderId);
    if (order) order->status = "Delivered";

    User* user = Common::findUser(pack->username);
    if (user) {
        user->score += pack->score;
        userService_.refreshUserHistoryStatus(*user);
    }

    wrFileService.saveUsersToFile();
    wrFileService.saveOrdersToFile();
    wrFileService.savePackagesToFile();

    std::cout << "Package status updated to Delivered. User score increased.\n";
}

void AdminService::adminMenu(User& adminUser) {
    (void)adminUser; // فعلاً استفاده نمی‌شود ولی برای سازگاری نگه داشته شده
    while (true) {
        std::cout << "\n=== Admin Menu ===\n"
                  << "1. View Products\n"
                  << "2. Add Product\n"
                  << "3. Delete Product\n"
                  << "4. Edit product\n"
                  << "5. View Users\n"
                  << "6. View Pending Packages\n"
                  << "7. Process Next Package\n"
                  << "8. Logout\n"
                  << "Select an option: ";

        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1") {
            displayProducts();
        } else if (choice == "2") {
            addProduct();
        } else if (choice == "3") {
            displayProducts();
            deleteProduct();
        } else if(choice == "4"){
            displayProducts();
            editProduct();
        }else if (choice == "5") {
            displayUsers();
        } else if (choice == "6") {
            displayPendingPackages();
        } else if (choice == "7") {
            processNextPackage();
        } else if (choice == "8") {
            std::cout << "Logging out...\n";
            break;
        } else {
            std::cout << "Invalid choice.\n";
        }
    }
}
