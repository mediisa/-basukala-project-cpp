#ifndef CUSTOMER_H
#define CUSTOMER_H


#include "Models.h"

class UserService;

class CustomerService {
    public:
    struct RemovedItem {
        OrderItem item;
        size_t rowIndex;
    };

    explicit CustomerService(UserService& userService);

    void customerMenu(User& user);

    private:
        UserService& userService_;


    bool askYesNo(const std::string& prompt);
    bool tryParseInt(const std::string& s, int& out);
    int totalQuantityInCartFor(const std::vector<OrderItem>& cart, const std::string& productName);
    int totalItem(const std::vector<OrderItem>& cart);
    void printCategoryOptions();
    
    
    void displayProducts();
    void selectProductByCategoryByNumber();
    void displayByTopSellingNamePriceProduct(int choice);

    
    void store(std::vector<OrderItem>& cart , User& user);
    void addToCart(std::vector<OrderItem>& cart);    
    void searchProductsWithSuggestions(std::vector<OrderItem>& cart);

    void addSelectedProductToCart(std::vector<OrderItem>& cart, Product* product);
    std::vector<Product> displayProductsByCategorySorted(const std::string& categoryFilter , bool& sortByPrice);
    Product* selectProductByName();
    Product* selectProductFromSuggestions(std::vector<std::string>& suggestions);
    Product* selectProductByCategoryByNumber1();

    void removeFromCart(std::vector<OrderItem>& cart, std::vector<RemovedItem>& removedStack);
    void undoLastRemoval(std::vector<OrderItem>& cart, std::vector<RemovedItem>& removedStack);
    void restoreRemovedByNumber(std::vector<OrderItem>& cart, std::vector<RemovedItem>& removedStack);

    void displayCart(const std::vector<OrderItem>& cart);
    void checkout(User& user, std::vector<OrderItem>& cart);
    void addFunds(User& user);

    void displayOrderHistoryFromOrders(const User& user);
    char promptDestinationCity();

    void displayTopSellingProduct(int topN);
    void displayUserInfo(User& user);

};
#endif