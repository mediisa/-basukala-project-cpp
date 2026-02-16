#include "customer.h"
#include "WRFile.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

void searchProductsWithSuggestions(){
    std::cout << "\n--- Product Search ---\n";
    std::string query;
    std::cout << "Enter product name or prefix: ";
    std::getline(std::cin, query);

    std::vector<std::string> suggestions = productTrie.suggest(query, 5);
    if (suggestions.empty()) {
        std::cout << "No matches found.\n";
        return;
    }
    std::cout << "Suggestions (prefix match):\n";
    for (size_t i = 0; i < suggestions.size(); ++i) {
        std::cout << i + 1 << ". " << suggestions[i] << "\n";
    }
}

Product* selectProductByName(){
    std::string name;
    std::cout << "Enter product name: ";
    std::getline(std::cin, name);
    Product* product = findProduct(name);
    if (!product) std::cout << "Product not found.\n";
    return product;
}

void addToCart(std::vector<OrderItem> &cart){
    std::cout << "\n--- Add to Cart ---\n";
    Product* product = selectProductByName();
    if (!product) return;
    if (product->stock <= 0) {
        std::cout << "Product out of stock.\n";
        return;
    }
    std::string qtyInput;
    std::cout << "Quantity: ";
    std::getline(std::cin, qtyInput);
    int qty = 0;
    try {
        qty = stoi(qtyInput);
    } catch (...) {
        std::cout << "Invalid quantity.\n";
        return;
    }
    if (qty <= 0) {
        std::cout << "Quantity must be positive.\n";
        return;
    }
    if (qty > product->stock) {
        std::cout << "Insufficient stock. Available: " << product->stock << "\n";
        return;
    }

    for (auto &item : cart) {
        if (item.productName == product->name) {
            if (item.quantity + qty > product->stock) {
                std::cout << "Total quantity exceeds available stock.\n";
                return;
            }
            item.quantity += qty;
            std::cout << "Updated cart entry.\n";
            return;
        }
    }

    OrderItem newItem;
    newItem.productName = product->name;
    newItem.quantity = qty;
    newItem.unitPrice = product->price;
    cart.push_back(newItem);
    std::cout << "Added to cart.\n";
}

void removeFromCart(std::vector<OrderItem> &cart){
    std::cout << "\n--- Remove from Cart ---\n";
    displayCart(cart);
    if (cart.empty()) return;
    std::string idxInput;
    std::cout << "Select item number to remove: ";
    std::getline(std::cin, idxInput);
    int index = 0;
    try {
        index = stoi(idxInput);
    } catch (...) { std::cout << "Invalid number.\n"; return; }
    if (index < 1 || index > static_cast<int>(cart.size())) {
        std::cout << "Out of range.\n";
        return;
    }
    cart.erase(cart.begin() + index - 1);
    std::cout << "Removed from cart.\n";
}

void displayCart(const std::vector<OrderItem> &cart){
    std::cout << "\n--- Shopping Cart ---\n";
    if (cart.empty()) {
        std::cout << "Cart is empty.\n";
        return;
    }
    double subtotal = 0.0;
    int totalItems = 0;
    for (size_t i = 0; i < cart.size(); ++i) {
        const auto &item = cart[i];
        double lineTotal = item.quantity * item.unitPrice;
        subtotal += lineTotal;
        totalItems += item.quantity;
        std::cout << i + 1 << ". " << item.productName
             << " | Quantity: " << item.quantity
             << " | Unit Price: $" << std::fixed << std::setprecision(2) << item.unitPrice
             << " | Line Total: $" << lineTotal << "\n";
    }
    std::cout << "Items in Cart: " << totalItems << "\n";
    std::cout << "Products Subtotal: $" << subtotal << "\n";
    std::cout << "Shipping Cost: $" << SHIPPING_COST << "\n";
    std::cout << "Grand Total: $" << subtotal + SHIPPING_COST << "\n";
}

void checkout(User &user, std::vector<OrderItem> &cart){
    if (cart.empty()) {
        std::cout << "Cart is empty. Nothing to checkout.\n";
        return;
    }
    displayCart(cart);
    double subtotal = 0.0;
    int totalItemCount = 0;
    for (const auto &item : cart) {
        subtotal += item.quantity * item.unitPrice;
        totalItemCount += item.quantity;
    }

    double totalCost = subtotal + SHIPPING_COST;

    std::cout << "Your current balance is $" << std::fixed << std::setprecision(2) << user.balance << "\n";
    if (user.balance < totalCost) {
        std::cout << "Insufficient balance. Please add funds.\n";
        return;
    }

    char destination = promptDestinationCity();
    if (destination == '\0') return;

    std::string address;
    std::cout << "Enter delivery address: ";
    std::getline(std::cin, address);
    if (address.empty()) {
        std::        cout << "Address cannot be empty.\n";
        return;
    }

    for (auto &item : cart) {
        Product* product = findProduct(item.productName);
        if (!product) {
            std::cout << "Product not found during checkout, aborting.\n";
            return;
        }

        if (product->stock < item.quantity) {
            std::cout << "Stock changed for " << product->name << ". Checkout cancelled.\n";
            return;
        }
        product->stock -= item.quantity;
        product->soldCount += item.quantity;
    }

    user.balance -= totalCost;


    std::string orderId = nextOrderId();
    OrderRecord newOrder;
    newOrder.id = orderId;
    newOrder.username = user.username;
    newOrder.items = cart;
    newOrder.totalCost = totalCost;
    newOrder.destinationCity = destination;
    newOrder.address = address;
    newOrder.status = "Pending";
    orders.push_back(newOrder);

    std::string packageId = nextPackageId();
    Package pack;
    pack.id = packageId;
    pack.orderId = orderId;
    pack.username = user.username;
    pack.items = cart;
    pack.destinationCity = destination;
    pack.address = address;
    pack.score = totalItemCount; 
    pack.status = "Pending";
    pack.routeDisplay = "Pending computation";
    pack.routeDistance = 0;
    pack.enqueueIndex = ++globalPackageSequence;
    packages.push_back(pack);

    std::stringstream histEntry;
    histEntry << "[" << orderId << "] "
              << "Items: " << totalItemCount
              << " | Total Paid: $" << std::fixed << std::setprecision(2) << totalCost
              << " | Destination: " << destination << " (" << CITY_LABELS.at(destination) << ")"
              << " | Status: Pending";
    user.orderHistory.push_back(histEntry.str());


    saveProductsToFile();
    saveUsersToFile();
    saveOrdersToFile();
    savePackagesToFile();

    cart.clear();
    std::cout << "Checkout completed. Order ID: " << orderId << ". Package queued for delivery.\n";
}

void addFunds(User &user){
    std::cout << "\n--- Add Funds ---\n";
    std::cout << "Current balance: $" << std::fixed << std::setprecision(2) << user.balance << "\n";
    std::string amountInput;
    std::cout << "Enter amount to add: ";
    std::getline(std::cin, amountInput);
    try {
        double amount = stod(amountInput);
        if (amount <= 0) {
            std::cout << "Amount must be positive.\n";
            return;
        }
        user.balance += amount;
        saveUsersToFile();
        std::cout << "Balance updated. New balance: $" << std::fixed << std::setprecision(2) << user.balance << "\n";
    } catch (...) {
        std::cout << "Invalid amount.\n";
    }
}

void customerMenu(User &user){
std::vector<OrderItem> cart;
    while (true) {
        std::cout << "\n=== Customer Menu ===\n"
             << "1. View Products\n"
             << "2. Search Products (Trie Suggestions)\n"
             << "3. Add Product to Cart\n"
             << "4. Remove Product from Cart\n"
             << "5. View Cart\n"
             << "6. Checkout\n"
             << "7. View Order History\n"
             << "8. Add Funds\n"
             << "9. Logout\n"
             << "Select an option: ";
        std::string choice;
        std::getline(std::cin, choice);
        if (choice == "1") {
            std::cout << "\n--- Product Catalog ---\n";
            if (products.empty()) {
                std::cout << "No products available.\n";
            } else {
                for (size_t i = 0; i < products.size(); ++i) {
                    const auto &p = products[i];
                    std::cout << i + 1 << ". " << p.name
                         << " | Price: $" << std::fixed << std::setprecision(2) << p.price
                         << " | Stock: " << p.stock
                         << " | Sold: " << p.soldCount << "\n";
                }
            }
        } else if (choice == "2") {
            searchProductsWithSuggestions();
        } else if (choice == "3") {
            addToCart(cart);
        } else if (choice == "4") {
            removeFromCart(cart);
        } else if (choice == "5") {
            displayCart(cart);
        } else if (choice == "6") {
            checkout(user, cart);
        } else if (choice == "7") {
            refreshUserHistoryStatus(user);
            std::cout << "\n--- Purchase History ---\n";
            if (user.orderHistory.empty()) {
                std::cout << "No orders recorded.\n";
            } else {
                for (const auto &entry : user.orderHistory) {
                    std::cout << entry << "\n";
                }
            }
        } else if (choice == "8") {
            addFunds(user);
        } else if (choice == "9") {
            std::cout << "Logging out...\n";
            break;
        } else {
            std::cout << "Invalid option.\n";
        }
    }
}


char promptDestinationCity(){
    std::cout << "\nAvailable cities:\n";
    for (const auto &[code, name] : CITY_LABELS) {
        std::cout << code << " - " << name;
        if (find(WAREHOUSE_CITIES.begin(), WAREHOUSE_CITIES.end(), code) != WAREHOUSE_CITIES.end()) {
            std::cout << " (Warehouse)";
        }
        std::cout << "\n";
    }
    std::string input;
    std::cout << "Enter destination city code: ";
    std::getline(std::cin, input);
    if (input.empty()) return '\0';
    char code = toupper(static_cast<unsigned char>(input[0]));
    if (!CITY_LABELS.count(code)) {
        std::cout << "Invalid city code.\n";
        return '\0';
    }
    return code;   
}