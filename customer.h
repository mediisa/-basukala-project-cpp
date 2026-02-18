#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "Models.h"

class UserService;

class CustomerService {
public:
    struct RemovedItem {
        OrderItem item;
        size_t originalIndex;
    };

    explicit CustomerService(UserService& userService);

    void customerMenu(User& user);

private:
    UserService& userService_;

    std::string promptLine(const std::string& prompt);
    bool askYesNo(const std::string& prompt);
    bool tryParseInt(const std::string& s, int& out);
    int totalQuantityInCartFor(const std::vector<OrderItem>& cart, const std::string& productName);

    void printCategoryOptions();
    std::vector<Product*> displayProductsByCategorySorted(const std::string& categoryFilter);

    void searchProductsWithSuggestions();
    Product* selectProductByName();
    Product* selectProductFromSuggestions();
    Product* selectProductByCategoryByNumber();

    void addToCart(std::vector<OrderItem>& cart);
    void addSelectedProductToCart(std::vector<OrderItem>& cart, Product* product);

    void removeFromCart(std::vector<OrderItem>& cart, std::vector<RemovedItem>& removedStack);
    void undoLastRemoval(std::vector<OrderItem>& cart, std::vector<RemovedItem>& removedStack);
    void restoreRemovedByNumber(std::vector<OrderItem>& cart, std::vector<RemovedItem>& removedStack);

    void displayCart(const std::vector<OrderItem>& cart);
    void checkout(User& user, std::vector<OrderItem>& cart);
    void addFunds(User& user);

    char promptDestinationCity();
};

//extern CustomerService customerService;

#endif