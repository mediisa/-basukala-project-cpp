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
            displayByTopSellingNamePriceProduct(1);
        }else if( m == "3"){
            displayByTopSellingNamePriceProduct(2);
        }else if( m == "4"){
            displayByTopSellingNamePriceProduct(3);
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
    std::vector<Product> list;

    if (!categoryFilter.empty()){
        list = productCatalog.getCategoryProducts(categoryFilter);
    }else{
        list = productCatalog.toVector();
    }
        


    std::sort(list.begin(), list.end(), [](const Product& a, const Product& b) {
        return Common::toUpperCopy(a.getname()) < Common::toUpperCopy(b.getname());
    });

    if (list.empty()) {
        if (!categoryFilter.empty()) std::cout << "No products found in the selected category.\n";
        else std::cout << "No products available.\n";
        return ;
    }

    for (size_t i = 0; i < list.size(); ++i) {
        const auto& product = list[i];
        std::cout << i  << ". " << product.getname()
                  << " | Category: " << (product.getcategory().empty() ? "-" : product.getcategory())
                  << " | Price: $" << std::fixed << std::setprecision(2) << product.getprice() 
                  << " | Stock: " << product.getstock() 
                  << " | Sold: " << product.getsoldCount() <<" \n";
    }

    return ;
}

void AdminService::selectProductByCategoryByNumber() {
    if (productCatalog.toVector().empty()) {
        std::cout << "No products available.\n";
        return ;
    }

    printCategoryOptions();
    std::string categoryInput = Common::promptLine("Filter by category (number or name, Enter for all): ");
    std::string resolvedCategory;

    if (!categoryInput.empty() && !Common::resolveCategoryInput(categoryInput, resolvedCategory)) {
        std::cout << "Invalid category selection. Showing all categories.\n";
        resolvedCategory.clear();
    }

    displayProductsByCategorySorted(resolvedCategory);

}

void AdminService::displayByTopSellingNamePriceProduct(int choice){
    auto sorted = productCatalog.toVector();
    if (sorted.empty()) {
        std::cout << "No products available.\n";
        return;
    }

    std::cout << "\n ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ \n";

    std::sort(sorted.begin() , sorted.end() , 
            [&choice](const Product& a , const Product& b){
                if(choice == 1){
                    return a.getsoldCount() > b.getsoldCount();
                }else if(choice == 2){
                    return a.getname() < b.getname();
                }else if(choice == 3){
                    return a.getprice() > b.getprice();
                }else{
                    return false;
                }
            });


    for (size_t i=0 ; i < sorted.size() ; ++i){
        const auto& p = sorted[i];
        std::cout << i+1 << ". "<<p.getname()
                  << " | Price: $"<< std::fixed << std::setprecision(2) << p.getprice()
                  << " | Category: "<<p.getcategory()
                  << " | Stock: " << p.getstock() 
                  << " | Sold: " << p.getsoldCount() <<" \n";
    }
}

void AdminService::displayUsers() {
    std::cout << "\n--- Registered Users ---\n";

    auto sorted = users;
    std::sort(sorted.begin() , sorted.end() , 
            [](const User& a , const User& b){
                return a.getusername() < b.getusername();
            });

    for (size_t i=0 ; i < users.size() ; ++i) {
        const auto& u = sorted[i];
        std::cout << "Username: " << u.getusername()
                  << " | Role: " << u.getrole()
                  << " | Balance: $" << std::fixed << std::setprecision(2) << u.getbalance()
                  << " | Score: " << u.getscore() << "\n";
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
    std::string countInput = Common::promptLine("How many items do you want to add ? \n");
    //std::cout << ;
    //std::getline(std::cin, countInput);

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
        std::string name = Common::promptLine("Product name: ");

        if (name.empty()) {
            std::cout << "Name cannot be empty.\n";
            return;
        }

        if (Common::findProduct(name)) {
            std::cout << "Product already exists.\n";
            while(true){
                std::string input = Common::promptLine("Do you want edit ?(Y/N) \n");
                if(Common::toUpperCopy(input) == "Y" ){ editProduct(); break;}
                else if(Common::toUpperCopy(input) == "N"){ return;}
                std::cout << "Please enter the Y/y or N/n.  ";
            }
        }

        std::string priceInput = Common::promptLine("Price (USD): ");
        std::string  stockInput = Common::promptLine("Stock quantity: ");
        //std::cout << "Price (USD): ";
        //std::getline(std::cin, priceInput);
        //std::cout << "Stock quantity: ";
        //std::getline(std::cin, stockInput);

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

            Product p(name,resolvedCategory,price,stock);

            productCatalog.insert(p);
            productTrie.build(productCatalog.toVector());
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
    std::string name = Common::promptLine("Product name to delete: ");
    //std::cout << ;
    //std::getline(std::cin, name);

    Product* prod = Common::findProduct(name);
    if(!prod){
        std::cout << "Product not found.\n"; return;
    }

    std::string oldCategory = prod->getcategory();
    std::string oldName = prod->getname();

    productCatalog.remove(oldCategory, oldName);
    productTrie.build(productCatalog.toVector());
    wrFileService.saveProductsToFile();
    std::cout << "Product removed.\n";

}

void AdminService::editProduct() {
    std::cout << "\n--- Edit Product ---\n";
    std::string name;
    std::cout << "Product name to edit: ";
    std::getline(std::cin, name);

    Product* prod = Common::findProduct(name);
    if(!prod){
        std::cout << "Product not found.\n"; return;
    }

    std::cout << "\n--- Modify Product ---\n"
              << "1) Edit price\n"
              << "2) Edit stock quantity\n"
              << "3) Edit name\n"
              << "4) Edit category\n"
              << "5) back\n"
              << "Choose: ";
    std::string m;
    std::getline(std::cin, m);

            if(m == "1"){
                std::string input = Common::promptLine("New price: ");
                try {
                    double newPrice = std::stod(input);
                    if (newPrice < 0) {
                        std::cout << "Price cannot be negative.\n";
                        return;
                    }
                    prod->setPrice(newPrice);
                    std::cout << "Price updated successfully.\n";
                    wrFileService.saveProductsToFile();
                    return;
                } catch (...) {
                    std::cout << "Invalid price.\n";
                    return;
                }
            }else if(m == "2"){
                std::string input = Common::promptLine("Stock quantity change (+/-): ");
                int delta = 0;

                if(!tryParseInt(input , delta)) {std::cout << "Invalid stock value.\n";  return;}

                if(prod->getstock() + delta < 0) {std::cout << "Stock cannot be negative.\n";  return;}

                if (delta > 0){
                    prod->addStock(delta);
                }else{
                    prod->reduceStock(-delta);
                }

                std::cout << "Stock quantity updated successfully.\n";
                wrFileService.saveProductsToFile();
                return;
            }else if(m == "3"){
                std::string newName = Common::promptLine("New product name: ");
                if(newName.empty()){
                    std::cout << "Name cannot be empty.\n"; return;
                }

                if (Common::findProduct(newName)) {
                    std::cout << "Another product with this name already exists.\n";
                    return;
                }

                Product updated = *prod;
                std::string oldCategory = prod->getcategory();
                std::string oldName = prod->getname();
                updated.setName(newName);

                productCatalog.remove(oldCategory, oldName);
                productCatalog.insert(updated);
                productTrie.build(productCatalog.toVector());
                wrFileService.saveProductsToFile();
                std::cout << "Name updated successfully.\n";
                return;
            }else if(m == "4"){
                printCategoryOptions();
                std::string input = Common::promptLine("New category (number or name): ");
                std::string resolved;
                if(!Common::resolveCategoryInput(input , resolved)){ std::cout << "Invalid category.\n"; return;}

                Product updated = *prod;
                std::string oldCategory = prod->getcategory();
                std::string oldName = prod->getname();
                updated.setCategory(resolved);

                productCatalog.remove(oldCategory, oldName);
                productCatalog.insert(updated);
                productTrie.build(productCatalog.toVector());                
                std::cout << "Categoyr updated successfully.\n";
                wrFileService.saveProductsToFile();
                return;
            }else if(m == "5"){
                return;
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

    std::cout << "\n--- Processing Package ---\n"
              << "Package ID: " << pack->id << "\n"
              << "Order ID: " << pack->orderId << "\n"
              << "Customer: " << pack->username << "\n"
              << "Destination: " << pack->destinationCity
              << " (" << CITY_LABELS.at(pack->destinationCity) << ")\n"
              << "Address: " << pack->address << "\n"
              << "Package Score (total items): " << pack->score << "\n"
              << "Products:\n";

    for (const auto& item : pack->items) {
        Product* prod = Common::findProduct(item.productName);
        std::cout << "  - " << item.productName;
        if (prod && !prod->getcategory().empty()) {
            std::cout << " [" << prod->getcategory() << "]";
        }
       // std::cout << item.quantity << "\n";
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
    if (order) order->setStatus("Delivered");

    User* user = Common::findUser(pack->username);
    if (user) {
        //user->score += pack->score;
        user->addScore(pack->score);
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
