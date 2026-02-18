#include "Customer.h"
#include "User.h"
#include "comm.h"
#include "WRFile.h"
#include "productTrie.h"
#include "Graph.h"
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

//CustomerService customerService;

CustomerService::CustomerService(UserService& userService)
    : userService_(userService) {}

std::string CustomerService::promptLine(const std::string& prompt) {
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    return input;
}

bool CustomerService::askYesNo(const std::string& prompt) {
    while (true) {
        std::string in = promptLine(prompt);
        if (in.empty()) continue;
        char c = std::toupper(static_cast<unsigned char>(in[0]));
        if (c == 'Y') return true;
        if (c == 'N') return false;
        std::cout << "Please enter Y or N.\n";
    }
}

bool CustomerService::tryParseInt(const std::string& s, int& out) {
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

int CustomerService::totalQuantityInCartFor(const std::vector<OrderItem>& cart, const std::string& productName) {
    int sum = 0;
    for (const auto& it : cart) {
        if (Common::equalsIgnoreCase(it.productName, productName)) sum += it.quantity;
    }
    return sum;
}

void CustomerService::printCategoryOptions() {
    std::cout << "Categories:\n";
    for (size_t i = 0; i < Common::PRODUCT_CATEGORIES.size(); ++i) {
        std::cout << "  " << i + 1 << ". " << Common::PRODUCT_CATEGORIES[i] << "\n";
    }
}

std::vector<Product*> CustomerService::displayProductsByCategorySorted(const std::string& categoryFilter) {
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
        return list;
    }

    for (size_t i = 0; i < list.size(); ++i) {
        const auto* product = list[i];
        std::cout << i + 1 << ". " << product->name
                  << " | Category: " << (product->category.empty() ? "-" : product->category)
                  << " | Price: $" << std::fixed << std::setprecision(2) << product->price
                  << " | Stock: " << product->stock << "\n";
                  ///<< " | Sold: " << product->soldCount << "\n";
    }

    return list;
}

void CustomerService::searchProductsWithSuggestions() {
    std::cout << "\n--- Product Search ---\n";
    std::string query = promptLine("Enter product name or prefix: ");

    std::vector<std::string> suggestions = productTrie.suggest(query, 10);
    if (suggestions.empty()) {
        std::cout << "No matches found.\n";
        return;
    }

    std::cout << "Suggestions (prefix match):\n";
    for (size_t i = 0; i < suggestions.size(); ++i) {
        std::cout << i + 1 << ". " << suggestions[i];
        Product* product = Common::findProduct(suggestions[i]);
        if (product && !product->category.empty()) {
            std::cout << " | Category: " << product->category;
        }
        std::cout << "\n";
    }
}

Product* CustomerService::selectProductFromSuggestions() {
    std::string in = promptLine("Enter suggestion number to select (-1 to go back): ");
    int idx = 0;
    if (!tryParseInt(in, idx)) { std::cout << "Invalid number.\n"; return nullptr; }
    if (idx == -1) return nullptr;

    std::cout << "Re-enter the search query to confirm selection: ";
    std::string query;
    std::getline(std::cin, query);

    auto suggestions = productTrie.suggest(query, 10);
    if (idx < 1 || idx > static_cast<int>(suggestions.size())) {
        std::cout << "Out of range.\n";
        return nullptr;
    }

    Product* p = Common::findProduct(suggestions[idx - 1]);
    if (!p) std::cout << "Selected product no longer exists.\n";
    return p;
}

Product* CustomerService::selectProductByName() {
    std::string name = promptLine("Enter product name: ");
    Product* product = Common::findProduct(name);
    if (!product) {
        std::cout << "Product not found.\n";
        return nullptr;
    }
    std::cout << "Selected: " << product->name
              << " | Category: " << (product->category.empty() ? "-" : product->category)
              << " | Price: $" << std::fixed << std::setprecision(2) << product->price << "\n";
    return product;
}

Product* CustomerService::selectProductByCategoryByNumber() {
    if (products.empty()) {
        std::cout << "No products available.\n";
        return nullptr;
    }

    printCategoryOptions();
    std::string categoryInput = promptLine("Filter by category (number or name, Enter for all): ");
    std::string resolvedCategory;

    if (!categoryInput.empty() && !Common::resolveCategoryInput(categoryInput, resolvedCategory)) {
        std::cout << "Invalid category selection. Showing all categories.\n";
        resolvedCategory.clear();
    }

    auto filtered = displayProductsByCategorySorted(resolvedCategory);
    if (filtered.empty()) return nullptr;

    std::string in = promptLine("Enter product number to select (-1 to go back): ");
    int idx = 0;
    if (!tryParseInt(in, idx)) { std::cout << "Invalid number.\n"; return nullptr; }
    if (idx == -1) return nullptr;
    if (idx < 1 || idx > static_cast<int>(filtered.size())) {
        std::cout << "Out of range.\n";
        return nullptr;
    }

    Product* p = filtered[idx - 1];
    std::cout << "Selected: " << p->name
              << " | Category: " << (p->category.empty() ? "-" : p->category)
              << " | Price: $" << std::fixed << std::setprecision(2) << p->price << "\n";
    return p;
}

void CustomerService::addSelectedProductToCart(std::vector<OrderItem>& cart, Product* product) {
    if (!product) return;
    if (product->stock <= 0) {
        std::cout << "Product out of stock.\n";
        return;
    }

    std::string qtyInput = promptLine("Quantity: ");
    int qty = 0;
    if (!tryParseInt(qtyInput, qty) || qty <= 0) {
        std::cout << "Invalid quantity.\n";
        return;
    }

    int existing = totalQuantityInCartFor(cart, product->name);
    if (existing + qty > product->stock) {
        std::cout << "Insufficient stock. Available: " << product->stock
                  << " | In cart: " << existing << " | Requested: " << qty << "\n";
        return;
    }

    OrderItem newItem(product->name, qty, product->price);
    cart.push_back(newItem);

    std::cout << "Added to cart as a new row.\n";
}

void CustomerService::addToCart(std::vector<OrderItem>& cart) {
    std::cout << "\n--- Add to Cart ---\n";
    std::string countInput = promptLine("How many different products do you want to add? ");
    int count = 0;
    if (!tryParseInt(countInput, count) || count <= 0) {
        std::cout << "Invalid number.\n";
        return;
    }

    for (int k = 0; k < count; ++k) {
        std::cout << "\nSelect method:\n"
                  << "  1) Select by category list (number)\n"
                  << "  2) Search (suggestions) and select by number\n"
                  << "  3) Enter product name directly\n"
                  << "  4) Back\n"
                  << "Choose: ";
        std::string m;
        std::getline(std::cin, m);

        if (m == "1") {
            Product* p = selectProductByCategoryByNumber();
            addSelectedProductToCart(cart, p);
        } else if (m == "2") {
            searchProductsWithSuggestions();
            if (askYesNo("Do you want to select a product from suggestions? (Y/N): ")) {
                Product* p = selectProductFromSuggestions();
                addSelectedProductToCart(cart, p);
            }
        } else if (m == "3") {
            Product* p = selectProductByName();
            addSelectedProductToCart(cart, p);
        } else if (m == "4") {
            return;
            //back
        } else {
            std::cout << "Invalid method. Skipped this item.\n";
        }
    }
}

void CustomerService::removeFromCart(std::vector<OrderItem>& cart, std::vector<RemovedItem>& removedStack) {
    std::cout << "\n--- Remove from Cart ---\n";
    displayCart(cart);
    if (cart.empty()) return;

    std::string idxInput = promptLine("Select item number to remove: ");
    int index = 0;
    if (!tryParseInt(idxInput, index)) { std::cout << "Invalid number.\n"; return; }
    if (index < 1 || index > static_cast<int>(cart.size())) {
        std::cout << "Out of range.\n";
        return;
    }

    RemovedItem r{cart[index - 1], static_cast<size_t>(index - 1)};
    removedStack.push_back(r);
    cart.erase(cart.begin() + index - 1);

    std::cout << "Removed from cart. You can undo it.\n";
}

void CustomerService::undoLastRemoval(std::vector<OrderItem>& cart, std::vector<RemovedItem>& removedStack) {
    if (removedStack.empty()) {
        std::cout << "Nothing to undo.\n";
        return;
    }

    RemovedItem r = removedStack.back();
    removedStack.pop_back();

    size_t pos = std::min(r.originalIndex, cart.size());
    cart.insert(cart.begin() + pos, r.item);

    std::cout << "Last removed item restored at position " << (pos + 1) << ".\n";
}

void CustomerService::restoreRemovedByNumber(std::vector<OrderItem>& cart, std::vector<RemovedItem>& removedStack) {
    if (removedStack.empty()) {
        std::cout << "No removed items to restore.\n";
        return;
    }

    std::cout << "\n--- Removed Items ---\n";
    for (size_t i = 0; i < removedStack.size(); ++i) {
        const auto& r = removedStack[i];
        std::cout << i + 1 << ". " << r.item.productName
                  << " | Qty: " << r.item.quantity
                  << " | Unit Price: $" << std::fixed << std::setprecision(2) << r.item.unitPrice
                  << " | Original Pos: " << (r.originalIndex + 1) << "\n";
    }

    std::string in = promptLine("Enter removed item number to restore (-1 to go back): ");
    int idx = 0;
    if (!tryParseInt(in, idx)) { std::cout << "Invalid number.\n"; return; }
    if (idx == -1) return;
    if (idx < 1 || idx > static_cast<int>(removedStack.size())) {
        std::cout << "Out of range.\n";
        return;
    }

    RemovedItem r = removedStack[idx - 1];
    removedStack.erase(removedStack.begin() + idx - 1);

    size_t pos = std::min(r.originalIndex, cart.size());
    cart.insert(cart.begin() + pos, r.item);

    std::cout << "Removed item restored at position " << (pos + 1) << ".\n";
}

void CustomerService::displayCart(const std::vector<OrderItem>& cart) {
    std::cout << "\n--- Shopping Cart ---\n";
    if (cart.empty()) {
        std::cout << "Cart is empty.\n";
        return;
    }

    double subtotal = 0.0;
    int totalItems = 0;
    for (size_t i = 0; i < cart.size(); ++i) {
        const auto& item = cart[i];
        Product* product = Common::findProduct(item.productName);
        std::string category = product ? product->category : "-";
        double lineTotal = item.quantity * item.unitPrice;
        subtotal += lineTotal;
        totalItems += item.quantity;

        std::cout << i + 1 << ". " << item.productName
                  << " | Category: " << category
                  << " | Quantity: " << item.quantity
                  << " | Unit Price: $" << std::fixed << std::setprecision(2) << item.unitPrice
                  << " | Line Total: $" << std::fixed << std::setprecision(2) << lineTotal << "\n";
    }

    std::cout << "Items in Cart: " << totalItems << "\n";
    std::cout << "Products Subtotal: $" << std::fixed << std::setprecision(2) << subtotal << "\n";
    std::cout << "Shipping Cost: $" << std::fixed << std::setprecision(2) << Common::SHIPPING_COST << "\n";
    std::cout << "Grand Total: $" << std::fixed << std::setprecision(2) << (subtotal + Common::SHIPPING_COST) << "\n";
}

void CustomerService::checkout(User& user, std::vector<OrderItem>& cart) {
    if (cart.empty()) {
        std::cout << "Cart is empty. Nothing to checkout.\n";
        return;
    }

    displayCart(cart);

    std::map<std::string, int> req;
    double subtotal = 0.0;
    int totalItemCount = 0;
    for (const auto& item : cart) {
        req[item.productName] += item.quantity;
        subtotal += item.quantity * item.unitPrice;
        totalItemCount += item.quantity;
    }

    for (const auto& kv : req) {
        Product* p = Common::findProduct(kv.first);
        if (!p) {
            std::cout << "Product " << kv.first << " not found during checkout. Aborting.\n";
            return;
        }
        if (p->stock < kv.second) {
            std::cout << "Stock changed for " << kv.first
                      << ". Needed: " << kv.second << ", Available: " << p->stock
                      << ". Checkout cancelled.\n";
            return;
        }
    }

    double totalCost = subtotal + Common::SHIPPING_COST;

    std::cout << "Your current balance is $" << std::fixed << std::setprecision(2) << user.balance << "\n";
    if (user.balance < totalCost) {
        std::cout << "Insufficient balance. Please add funds.\n";
        return;
    }

    char destination = promptDestinationCity();
    if (destination == '\0') return;

    std::string address = promptLine("Enter delivery address: ");
    if (address.empty()) {
        std::cout << "Address cannot be empty.\n";
        return;
    }

    for (const auto& kv : req) {
        Product* p = Common::findProduct(kv.first);
        p->stock -= kv.second;
        p->soldCount += kv.second;
    }

    user.balance -= totalCost;

    std::string orderId = Common::nextOrderId();
    Order newOrder;
    newOrder.id = orderId;
    newOrder.username = user.username;
    newOrder.items = cart;
    newOrder.totalCost = totalCost;
    newOrder.destinationCity = destination;
    newOrder.address = address;
    newOrder.status = "Pending";
    orders.push_back(newOrder);

    std::string packageId = Common::nextPackageId();
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
    pack.enqueueIndex = Common::getPackageSequence();
    packages.push_back(pack);

    std::stringstream histEntry;
    histEntry << "[" << orderId << "] "
              << "Items: " << totalItemCount
              << " | Total Paid: $" << std::fixed << std::setprecision(2) << totalCost
              << " | Destination: " << destination << " (" << CITY_LABELS.at(destination) << ")"
              << " | Status: Pending";
    user.orderHistory.push_back(histEntry.str());

    wrFileService.saveAll();

    cart.clear();
    std::cout << "Checkout completed. Order ID: " << orderId << ". Package queued for delivery.\n";
}

void CustomerService::addFunds(User& user) {
    std::cout << "\n--- Add Funds ---\n";
    std::cout << "Current balance: $" << std::fixed << std::setprecision(2) << user.balance << "\n";

    std::string amountInput = promptLine("Enter amount to add: ");
    try {
        double amount = std::stod(amountInput);
        if (amount <= 0) {
            std::cout << "Amount must be positive.\n";
            return;
        }
        user.balance += amount;
        wrFileService.saveUsersToFile();
        std::cout << "Balance updated. New balance: $" << std::fixed << std::setprecision(2) << user.balance << "\n";
    } catch (...) {
        std::cout << "Invalid amount.\n";
    }
}

void CustomerService::customerMenu(User& user) {
    std::vector<OrderItem> cart;
    std::vector<RemovedItem> removedStack;

    while (true) {
        std::cout << "\n=== Customer Menu ===\n"
                  << "1. View Products (select by number)\n"
                  << "2. Search Products (Trie) and select\n"
                  << "3. Add Product to Cart (by name/category/search)\n"
                  << "4. Modify Cart (remove / undo / restore by number)\n"
                  << "5. View Cart\n"
                  << "6. Checkout\n"
                  << "7. View Order History\n"
                  << "8. Add Funds\n"
                  << "9. Logout\n"
                  << "Select an option: ";

        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1") {
            Product* p = selectProductByCategoryByNumber();
            if (p && askYesNo("Add selected product to cart? (Y/N): ")) {
                addSelectedProductToCart(cart, p);
            }
        } else if (choice == "2") {
            searchProductsWithSuggestions();
            if (askYesNo("Do you want to select a product from suggestions? (Y/N): ")) {
                Product* p = selectProductFromSuggestions();
                if (p && askYesNo("Add selected product to cart? (Y/N): ")) {
                    addSelectedProductToCart(cart, p);
                }
            }
        } else if (choice == "3") {
            addToCart(cart);
            if (askYesNo("Do you want to settle the bill now? (Y/N): ")) {
                checkout(user, cart);
            }
        } else if (choice == "4") {
            std::cout << "\n--- Modify Cart ---\n"
                      << "1) Remove item by number\n"
                      << "2) Undo last removal\n"
                      << "3) Restore removed item by number\n"
                      << "4) Back\n"
                      << "Choose: ";
            std::string m;
            std::getline(std::cin, m);

            if (m == "1") {
                displayCart(cart);
                removeFromCart(cart, removedStack);
            } else if (m == "2") {
                undoLastRemoval(cart, removedStack);
            } else if (m == "3") {
                restoreRemovedByNumber(cart, removedStack);
            } else if (m == "4") {
                // back
            } else {
                std::cout << "Invalid option.\n";
            }
        } else if (choice == "5") {
            displayCart(cart);
        } else if (choice == "6") {
            checkout(user, cart);
        } else if (choice == "7") {
            userService_.refreshUserHistoryStatus(user);
            std::cout << "\n--- Purchase History ---\n";
            if (user.orderHistory.empty()) {
                std::cout << "No orders recorded.\n";
            } else {
                for (const auto& entry : user.orderHistory) {
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

char CustomerService::promptDestinationCity() {
    std::cout << "\nAvailable cities:\n";
    for (const auto& kv : CITY_LABELS) {
        char code = kv.first;
        const std::string& name = kv.second;
        std::cout << code << " - " << name;
        if (std::find(WAREHOUSE_CITIES.begin(), WAREHOUSE_CITIES.end(), code) != WAREHOUSE_CITIES.end()) {
            std::cout << " (Warehouse)";
        }
        std::cout << "\n";
    }

    std::string input = promptLine("Enter destination city code: ");
    if (input.empty()) return '\0';
    char code = std::toupper(static_cast<unsigned char>(input[0]));

    if (!CITY_LABELS.count(code)) {
        std::cout << "Invalid city code.\n";
        return '\0';
    }
    return code;
}
