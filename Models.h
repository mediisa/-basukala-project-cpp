#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <vector>

class OrderItem {
public:
    std::string productName;
    int quantity;
    double unitPrice;

    OrderItem();
    OrderItem(const std::string& name, int qty, double price);

    double lineTotal() const;
};

class Product {
public:
    std::string name;
    std::string category;
    double price;
    int stock;
    int soldCount;

    Product();
    Product(const std::string& name, const std::string& category, double price, int stock);

    bool isInStock(int qty) const;
    void reduceStock(int qty);
    void addStock(int qty);
    void addSold(int qty);

    void setPrice(double p);
    void setName(const std::string& n);
    void setCategory(const std::string& c);
};

class Order {
public:
    std::string id;
    std::string username;
    std::vector<OrderItem> items;
    double totalCost;
    char destinationCity;
    std::string address;
    std::string status;

    Order();

    int totalItems() const;
    double subtotal() const;
    void recomputeTotal(double shippingCost);
};

class Package {
public:
    std::string id;
    std::string orderId;
    std::string username;
    std::vector<OrderItem> items;
    char destinationCity;
    std::string address;
    int score;
    std::string status;
    std::string routeDisplay;
    int routeDistance;
    long long enqueueIndex;

    Package();

    int computeScoreFromItems() const;
    void updateRoute(const std::string& routeText, int distance);
};

class User {
public:
    std::string username;
    std::string password;
    std::string role;

    double balance;
    int score;
    std::vector<std::string> orderHistory;

    User();
    User(const std::string& username, const std::string& password, const std::string& role);
};

#endif