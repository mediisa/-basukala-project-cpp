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
#include <queue>
#include <map>



CustomerService::CustomerService(UserService& userService)
    : userService_(userService) {}


bool CustomerService::askYesNo(const std::string& prompt) {
    while (true) {
        std::string in = Common::promptLine(prompt);
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

int CustomerService::totalItem(const std::vector<OrderItem>& cart){
    int totalItem=0;
    for(size_t i=0 ; i < cart.size() ; ++i){
        auto& item = cart[i];
        totalItem += item.quantity;
    }
    return totalItem;
}

void CustomerService::printCategoryOptions() {
    std::cout << "Categories:\n";
    for (size_t i = 0; i < Common::PRODUCT_CATEGORIES.size(); ++i) {
        std::cout << "  " << i + 1 << ". " << Common::PRODUCT_CATEGORIES[i] << "\n";
    }
}

void CustomerService::customerMenu(User& user) {
    std::vector<OrderItem> cart;
    std::vector<RemovedItem> removedStack;

    displayTopSellingProduct(5);
    displayUserInfo(user);

    while(true) {
        std::cout << "\n=== Customer Menu ===\n"
                  << "1. View Products(4 metods)\n"
                  << "2. Store(add to cart/view cart/search/checkout)\n"
                  << "3. Modify Cart (remove / undo / restore by number)\n"
                  << "4. View Order History\n"
                  << "5. Add Funds\n"
                  << "6. View Cart\n"
                  << "7. Checkout\n"
                  << "8. Logout\n"
                  << "Select an option: ";

        std::string choice;
        std::getline(std::cin, choice);

        if(choice == "1"){
            displayProducts();
               if (askYesNo("Do you want to select a product from suggestions? (Y/N): ")) {
                    std::string name = Common::promptLine("Enter Product Name: ");
                    Product* p = Common::findProduct(name);
                    addSelectedProductToCart(cart, p);
                }
        }else if(choice == "2"){
            store(cart , user );
        }else if(choice == "3"){
            std::cout << "\n--- Modify Cart ---\n"
                      << "1) Remove item by number\n"
                      << "2) Undo last removal\n"
                      << "3) Restore removed item by number\n"
                      << "4) Back\n"
                      << "Choose: ";
            std::string m;
            std::getline(std::cin, m);

            if (m == "1") {
                removeFromCart(cart, removedStack);
            } else if (m == "2") {
                undoLastRemoval(cart, removedStack);
            } else if (m == "3") {
                restoreRemovedByNumber(cart, removedStack);
            } else if (m == "4") {
                return;
            } else {
                std::cout << "Invalid option.\n";
            }
        }else if(choice == "4") {
            //userService_.refreshUserHistoryStatus(user);
            displayOrderHistoryFromOrders(user);
        }else if(choice == "5"){
            addFunds(user);
        }else if(choice == "6"){
            displayCart(cart);
        }else if(choice == "7"){
            checkout(user, cart);
        }else if(choice == "8"){
            std::cout << "Logging out...\n";
            break;
        }else{
            std::cout << "Invalid option.\n";
        }
    }
}

void CustomerService::displayProducts() {
        std::cout << "\n--- displayProducts ---\n"
                  << "  Select method:\n"
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

void CustomerService::store(std::vector<OrderItem>& cart , User& user ){
    while(true){
        std::cout << "\n=== Store Menu ===\n"
                  << "1. Add Product to Cart (by name/category/search)\n"
                  << "2. View cart\n"
                  << "3. Search Products (Trie) and select\n"
                  << "4. checkout\n"
                  << "5. Back\n"
                  << "Choose: ";

        std::string m;
        std::getline(std::cin, m);  
        
        if(m == "1"){
            addToCart(cart);
        }else if(m == "2"){
            displayCart(cart);
        }else if(m == "3"){
            searchProductsWithSuggestions(cart);       
        }else if(m == "4"){
            checkout(user, cart);
        }else if(m == "5"){
            return;
        }else{
            std::cout << "Invalid option.\n";
        }
    }
}

void CustomerService::addToCart(std::vector<OrderItem>& cart) {
    std::cout << "\n--- Add to Cart ---\n";
    std::string countInput = Common::promptLine("How many different products do you want to add? ");
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
            Product* p = selectProductByCategoryByNumber1();
            addSelectedProductToCart(cart, p);
        } else if (m == "2") {
            searchProductsWithSuggestions(cart);
        } else if (m == "3") {
            Product* p = selectProductByName();
            addSelectedProductToCart(cart, p);
        } else if (m == "4") {
            return;
        } else {
            std::cout << "Invalid method. Skipped this item.\n";
        }
    }
}

std::vector<Product> CustomerService::displayProductsByCategorySorted(const std::string& categoryFilter , bool& sortByPrice) {
    std::vector<Product> list;

    if (!categoryFilter.empty()){
        list = productCatalog.getCategoryProducts(categoryFilter);
    }else{
        list = productCatalog.toVector();
    }

    if(sortByPrice){
        std::sort(list.begin(), list.end(), [](const Product& a, const Product& b) {
        return a.getprice() > b.getprice();
        });        
    }else{
        std::sort(list.begin(), list.end(), [](const Product& a, const Product& b) {
            return Common::toUpperCopy(a.getname()) < Common::toUpperCopy(b.getname());
        });
    }

    if (list.empty()) {
        if (!categoryFilter.empty()) std::cout << "No products found in the selected category.\n";
        else std::cout << "No products available.\n";
        return list;
    }

    for (size_t i = 0; i < list.size(); ++i) {
        const auto& product = list[i];
        std::cout << i  << ". " << product.getname()
                  << " | Category: " << (product.getcategory().empty() ? "-" : product.getcategory())
                  << " | Price: $" << std::fixed << std::setprecision(2) << product.getprice() <<" \n";
    }

    return list;
}

void CustomerService::selectProductByCategoryByNumber() {
    if (productCatalog.toVector().empty()) {
        std::cout << "No products available.\n";
        return ;
    }

    printCategoryOptions();
    std::string categoryInput = Common::promptLine("Filter by category (number or name, Enter for all): ");
    std::string resolvedCategory;
    bool sortByPrice = false;

    if (!categoryInput.empty() && !Common::resolveCategoryInput(categoryInput, resolvedCategory)) {
        std::cout << "Invalid category selection. Showing all categories.\n";
        resolvedCategory.clear();
        sortByPrice = true;
    }

    displayProductsByCategorySorted(resolvedCategory , sortByPrice);
    

        std::string cmd = Common::promptLine("For price sort enter -2, Back enter -1: ");
        if(cmd == "-1"){
            return;
        }else if( cmd == "-2"){
            sortByPrice = true;
            displayProductsByCategorySorted(resolvedCategory , sortByPrice);     
        }else {
            std::cout << "Invalid command.\n";
            return; 
        }
}

void CustomerService::displayByTopSellingNamePriceProduct(int choice){
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
                  << " | Category: "<<p.getcategory() <<" \n";
    }
}

void CustomerService::displayTopSellingProduct(int topN){
    auto sorted = productCatalog.toVector();
    if (sorted.empty()) {
        std::cout << "No products available.\n";
        return;
    }

    std::cout << "\n \n ~ ~ ~ ~ ~ Top Selling Products ~ ~ ~ ~ ~ \n";

    /*std::sort(sorted.begin() , sorted.end() , 
            [](const Product& a , const Product& b){
                return a.getsoldCount() > b.getsoldCount();
            });*/

    auto cmp = [](const Product& a, const Product& b){
        return a.getsoldCount() < b.getsoldCount(); 
    };

    std::priority_queue<Product, std::vector<Product>, decltype(cmp)> pq(cmp);

    for(const auto& p : sorted ){
        pq.push(p);
    }

    int lim = std::min(topN , static_cast<int>(sorted.size()));
    for (int i=0 ; i<lim ; ++i){
        const auto& p = pq.top();
        std::cout << i+1 << ". "<<p.getname()
                  << " | Price: $"<< std::fixed << std::setprecision(2) << p.getprice()
                  << " | Category: "<<p.getcategory() <<"\n";
        
        pq.pop();
    }
}

void CustomerService::displayUserInfo(User& user){
    std::cout << "\n ---- User ---- \n"
              << "Username: "<<user.getusername()
              << " | Password: "<<user.getpassword()
              << " | Balance: $"<<user.getbalance()
              << " | Score: "<<user.getscore()<<"\n";
}

void CustomerService::displayCart(const std::vector<OrderItem>& cart) {
    std::cout << "\n--- Shopping Cart ---\n";
    if (cart.empty()) {
        std::cout << "Cart is empty.\n";
        return;
    }

    double subtotal = 0.0;
    int J = 0;
    for (size_t i = 0; i < cart.size(); ++i) {
        const auto& item = cart[i];
        Product* product = Common::findProduct(item.productName);
        std::string category = product ? product->getcategory() : "-";
        double lineTotal = item.quantity * item.unitPrice;
        subtotal += lineTotal;
        int a=item.quantity;
        while(!(a < 1)){
            std::cout << i+J << ". " << item.productName
                    << " | Category: " << category
                    << " | Quantity: 1 \n" ;
            a--;
            J++;         
        }
        J--;
        std::cout <<" | Unit Price: $" << std::fixed << std::setprecision(2) << item.unitPrice
                  << " | Line Total: $" << std::fixed << std::setprecision(2) << lineTotal << "\n";
    }

    std::cout << "Items in Cart: " << totalItem(cart) << "\n";
    std::cout << "Products Subtotal: $" << std::fixed << std::setprecision(2) << subtotal << "\n";
    std::cout << "Shipping Cost: $" << std::fixed << std::setprecision(2) << Common::SHIPPING_COST << "\n";
    std::cout << "Grand Total: $" << std::fixed << std::setprecision(2) << (subtotal + Common::SHIPPING_COST) << "\n";
    
    
}

void CustomerService::searchProductsWithSuggestions(std::vector<OrderItem>& cart) {
    std::cout << "\n--- Product Search ---\n";
    std::string query = Common::promptLine("Enter product name or prefix: ");

    std::vector<std::string> suggestions = productTrie.suggest(query, 10);
    if (suggestions.empty()) {
        std::cout << "No matches found.\n";
        return;
    }

    std::cout << "Suggestions (prefix match):\n";
    for (size_t i = 0; i < suggestions.size(); ++i) {
        std::cout << i  << ". " << suggestions[i];
        Product* product = Common::findProduct(suggestions[i]);
        if (product && !product->getcategory().empty()) {
            std::cout << " | Category: " << product->getcategory();
        }
        std::cout << "\n";
    }

   if (askYesNo("Do you want to select a product from suggestions? (Y/N): ")) {
        Product* p = selectProductFromSuggestions(suggestions);
        addSelectedProductToCart(cart, p);
    }
}

Product* CustomerService::selectProductFromSuggestions(std::vector<std::string>& suggestions) {
    std::string in = Common::promptLine("Enter suggestion number to select (-1 to go back): ");
    int idx = 0;
    if (!tryParseInt(in, idx)) { std::cout << "Invalid number.\n"; return nullptr; }
    if (idx == -1) return nullptr;

    if (idx < 0 || idx >= static_cast<int>(suggestions.size())) {
        std::cout << "Out of range.\n";
        return nullptr;
    }
  
    Product* p = Common::findProduct(suggestions[idx]);
    if (!p) std::cout << "Selected product no longer exists.\n";
 
    return p;
}

Product* CustomerService::selectProductByName() {
    std::string name = Common::promptLine("Enter product name: ");
    Product* product = Common::findProduct(name);
    if (!product) {
        std::cout << "Product not found.\n";
        return nullptr;
    }
    std::cout << "Selected: " << product->getname()
              << " | Category: " << (product->getcategory().empty() ? "-" : product->getcategory())
              << " | Price: $" << std::fixed << std::setprecision(2) << product->getprice() << "\n";
    return product;
}

Product* CustomerService::selectProductByCategoryByNumber1() {
    if (productCatalog.toVector().empty()) {
        std::cout << "No products available.\n";
        return nullptr;
    }

    printCategoryOptions();
    std::string categoryInput = Common::promptLine("Filter by category (number or name, Enter for all): ");
    std::string resolvedCategory;

    if (!categoryInput.empty() && !Common::resolveCategoryInput(categoryInput, resolvedCategory)) {
        std::cout << "Invalid category selection. Showing all categories.\n";
        resolvedCategory.clear();
    }

    bool sortByPrice = false;
    auto filtered = displayProductsByCategorySorted(resolvedCategory , sortByPrice);
    if (filtered.empty()) return nullptr;

    std::string in = Common::promptLine("Enter product number to select (-1 to go back): ");
    int idx = 0;
    if (!tryParseInt(in, idx)) { std::cout << "Invalid number.\n"; return nullptr; }
    if (idx == -1) return nullptr;
    if (idx < 0 || idx >= static_cast<int>(filtered.size())) {
        std::cout << "Out of range.\n";
        return nullptr;
    }

    Product* p = Common::findProduct(filtered[idx].getname());
    std::cout << "Selected: " << p->getname()
              << " | Category: " << (p->getcategory().empty() ? "-" : p->getcategory())
              << " | Price: $" << std::fixed << std::setprecision(2) << p->getprice() << "\n";
    return p;
}

void CustomerService::addSelectedProductToCart(std::vector<OrderItem>& cart, Product* product) {
    if (!product) return;
    if (product->getstock() <= 0) {
        std::cout << "Product out of stock.\n";
        return;
    }

    std::string qtyInput = Common::promptLine("Quantity: ");
    int qty = 0;
    if (!tryParseInt(qtyInput, qty) || qty <= 0) {
        std::cout << "Invalid quantity.\n";
        return;
    }

    int existing = totalQuantityInCartFor(cart, product->getname());
    if (existing + qty > product->getstock()) {
        std::cout << "Insufficient stock. Available: " << product->getstock()
                  << " | In cart: " << existing << " | Requested: " << qty << "\n";
        return;
    }

    OrderItem newItem(product->getname(), qty, product->getprice());
    cart.push_back(newItem);

    std::cout << "Added to cart as a new row.\n";
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
        if (p->getstock() < kv.second) {
            std::cout << "Stock changed for " << kv.first
                      << ". Needed: " << kv.second << ", Available: " << p->getstock()
                      << ". Checkout cancelled.\n";
            return;
        }
    }

    double totalCost = subtotal + Common::SHIPPING_COST;

    std::cout << "Your current balance is $" << std::fixed << std::setprecision(2) << user.getbalance() << "\n";
    if (user.getbalance() < totalCost) {
        std::cout << "Insufficient balance. Please add funds.\n";
        return;
    }

    char destination = promptDestinationCity();
    if (destination == '\0') return;

    std::string address = Common::promptLine("Enter delivery address: ");
    if (address.empty()) {
        std::cout << "Address cannot be empty.\n";
        return;
    }

    for (const auto& kv : req) {
        Product* p = Common::findProduct(kv.first);
        p->reduceStock(kv.second);
        p->addSold(kv.second);
    }

    user.reduceBlance(totalCost);
    User* admin = Common::findUser("Admin");
    if (admin) {
        admin->addBlance(totalCost);
    }
    
    std::string orderId = Common::nextOrderId();
    Order newOrder(orderId , user.getusername() , cart , totalCost , destination , address , "Pending" );
    /*newOrder.id = orderId;
    newOrder.username = user.getusername();
    newOrder.items = cart;
    newOrder.totalCost = totalCost;
    newOrder.destinationCity = destination;
    newOrder.address = address;
    newOrder.status = "Pending";*/
    orders.push_back(newOrder);

    std::string packageId = Common::nextPackageId();
    Package pack(packageId , orderId , user.getusername() , cart , destination , address , "Pending" , "Pending computation");
    pack.setscore(totalItemCount);
    pack.setrouteDistance(0);
    pack.setenqueueIndex(Common::getPackageSequence());
    //pack.id = packageId;
    //pack.orderId = orderId;
   // pack.username = user.getusername();
    //pack.items = cart;
   // pack.destinationCity = destination;
   // pack.address = address;
    //pack.score = totalItemCount;
    //pack.status = "Pending";
    //pack.routeDisplay = "Pending computation";
   // pack.routeDistance = 0;
   // pack.enqueueIndex = Common::getPackageSequence();
    packages.push_back(pack);

    std::stringstream histEntry;
    histEntry << "[" << orderId << "] "
              << "Items: " << totalItemCount
              << " | Total Paid: $" << std::fixed << std::setprecision(2) << totalCost
              << " | Destination: " << destination << " (" << CITY_LABELS.at(destination) << ")"
              << " | Status: Pending";
    

    wrFileService.saveAll();

    cart.clear();
    std::cout << "Checkout completed. Order ID: " << orderId << ". Package queued for delivery.\n";
}

void CustomerService::addFunds(User& user) {
    std::cout << "\n--- Add Funds ---\n";
    std::cout << "Current balance: $" << std::fixed << std::setprecision(2) << user.getbalance() << "\n";

    std::string amountInput = Common::promptLine("Enter amount to add: ");
    try {
        double amount = std::stod(amountInput);
        if (amount <= 0) {
            std::cout << "Amount must be positive.\n";
            return;
        }
        //user.balance += amount;
        user.addBlance(amount);
        wrFileService.saveUsersToFile();
        std::cout << "Balance updated. New balance: $" << std::fixed << std::setprecision(2) << user.getbalance() << "\n";
    } catch (...) {
        std::cout << "Invalid amount.\n";
    }
}

void CustomerService::displayOrderHistoryFromOrders(const User& user) {
    std::cout << "\n--- Purchase History ---\n";

    bool found = false;

    for (const auto& order : orders) {
        if (order.getusername() != user.getusername())
            continue;

        found = true;

        int totalItems = 0 , index = 1;
        for (const auto& it : order.getitems())
            totalItems += it.quantity;

        std::cout << "#######\nOrder Id: [" << order.getid() << "] \n"
                  << "Total Items: " << totalItems << "\nProductName: \n";
        
            for (const auto& item : order.getitems()){
                std::cout << "\t" <<item.productName << "\n";
            }
        std::cout << "| Total Paid: $" << std::fixed << std::setprecision(2) << order.gettotalCost()
                  << "\n| Destination: " << order.getdestinationCity()
                  << " (" << CITY_LABELS.at(order.getdestinationCity()) << ")"
                  << "\n| Status: " << order.getstatus()
                  << "\n";
    }

    if (!found) {
        std::cout << "No orders recorded.\n";
    }
}

void CustomerService::removeFromCart(std::vector<OrderItem>& cart, std::vector<RemovedItem>& removedStack) {
    std::cout << "\n--- Remove from Cart ---\n";
    displayCart(cart);
    if (cart.empty()) return;

    std::string idxInput = Common::promptLine("Select item number to remove: ");
    int index = 0;
    if (!tryParseInt(idxInput, index)) { std::cout << "Invalid number.\n"; return; }

    int total = static_cast<int>(totalItem(cart));
    if (index < 0 || index >= total) {
        std::cout << "Out of range.\n";
        return;
    }

    int current = 0;
    for (size_t row = 0; row < cart.size(); ++row) {
        int rowQty = cart[row].quantity;


        if (index < current + rowQty) {
            RemovedItem r;
            r.item = cart[row];
            r.item.quantity = 1;
            r.rowIndex = row;

            removedStack.push_back(r);

            if (cart[row].quantity > 1) {
                cart[row].quantity -= 1;   
            } else {
                cart.erase(cart.begin() + row );
            }

            std::cout << "Removed item " << index << " from cart.\n";
            return;
        }
        current += rowQty;
    }

}

void CustomerService::undoLastRemoval(std::vector<OrderItem>& cart, std::vector<RemovedItem>& removedStack) {
    if (removedStack.empty()) {
        std::cout << "Nothing to undo.\n";
        return;
    }

    RemovedItem r = removedStack.back();
    removedStack.pop_back();

    for(auto& item : cart){
        if(Common::equalsIgnoreCase(item.productName , r.item.productName) && item.unitPrice == r.item.unitPrice){
            item.quantity += 1;
            std::cout << "Restored 1 unit of " << item.productName << ".\n";
            return;
        }
    }
    OrderItem restored = r.item;
    restored.quantity = 1;

    size_t total=totalItem(cart);
    size_t pos = std::min(r.rowIndex, total);
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
        std::cout << i  << ". " << r.item.productName
                  << " | Qty: " << r.item.quantity
                  << " | Unit Price: $" << std::fixed << std::setprecision(2) << r.item.unitPrice << "\n";
                
    }

    std::string in = Common::promptLine("Enter removed item number to restore (-1 to go back): ");
    int idx = 0;
    if (!tryParseInt(in, idx)) { std::cout << "Invalid number.\n"; return; }
    if (idx == -1) return;
    if (idx < 0 || idx >= static_cast<int>(removedStack.size())) {
        std::cout << "Out of range.\n";
        return;
    }

    RemovedItem r = removedStack[idx];
    removedStack.erase(removedStack.begin() + idx );

    for (auto& item : cart) {
        if (Common::equalsIgnoreCase(item.productName, r.item.productName)
            && item.unitPrice == r.item.unitPrice) {
            item.quantity += 1;
            std::cout << "Restored 1 unit of " << item.productName << ".\n";
            return;
        }
    }
    OrderItem restored = r.item;
    restored.quantity = 1;

    size_t total=totalItem(cart);
    size_t pos = std::min(r.rowIndex, cart.size());
    cart.insert(cart.begin() + pos, r.item);


    std::cout << "Removed item restored at position \n" ;
}

char CustomerService::promptDestinationCity() {
    std::cout << "\nAvailable cities:\n";

    std::vector<char> keys;
    keys.reserve(CITY_LABELS.size());

    for (const auto& kv : CITY_LABELS) {
        keys.push_back(kv.first);
    }
    std::sort(keys.begin(), keys.end());

    for (char code : keys) {
        const std::string& name = CITY_LABELS.at(code);
        std::cout << code << " - " << name;
        if (std::find(WAREHOUSE_CITIES.begin(), WAREHOUSE_CITIES.end(), code) != WAREHOUSE_CITIES.end()) {
            std::cout << " (Warehouse)";
        }
        std::cout << "\n";
    }

    std::string input = Common::promptLine("Enter destination city code: ");
    if (input.empty()) return '\0';
    char code = std::toupper(static_cast<unsigned char>(input[0]));

    if (!CITY_LABELS.count(code)) {
        std::cout << "Invalid city code.\n";
        return '\0';
    }
    return code;
}
