#include "customer.h"

#include <sstream>
#include <iomanip>

void searchProductsWithSuggestions(){

}

Product* selectProductByName(){

}

void addToCart(std::vector<OrderItem> &cart){

}

void removeFromCart(std::vector<OrderItem> &cart){

}

void displayCart(const std::vector<OrderItem> &cart){

}

void checkout(User &user, std::vector<OrderItem> &cart){

}

void addFunds(User &user){

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

}