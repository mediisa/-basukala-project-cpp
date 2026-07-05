#ifndef COMM_H
#define COMM_H

#include "Models.h"
#include "ProductBST.h"
#include <string>
#include <vector>


extern std::vector<User> users;
//extern std::vector<Product> products;
extern ProductBST productCatalog;
extern std::vector<Order> orders;
extern std::vector<Package> packages;

struct PackageComparator {
    bool operator()(Package* a, Package* b) const;
};

namespace Common {
    extern const double SHIPPING_COST;
    extern const std::string USERS_FILE;
    extern const std::string PRODUCTS_FILE;
    extern const std::string ORDERS_FILE;
    extern const std::string PACKAGES_FILE;

    extern const std::vector<std::string> PRODUCT_CATEGORIES;
    std::string promptLine(const std::string& prompt);

    std::vector<std::string> split(const std::string& line, char delimiter);
    std::string join(const std::vector<std::string>& items, char delimiter);
    std::string trim(const std::string& s);



    std::string toUpperCopy(const std::string& input);
    bool equalsIgnoreCase(const std::string& a, const std::string& b);
    bool resolveCategoryInput(const std::string& input, std::string& resolvedCategory);

    void setOrderSequence(int value);
    void setPackageSequence(int value);
    //int getOrderSequence();
    int getPackageSequence();


    std::string nextOrderId();
    std::string nextPackageId();

    User* findUser(const std::string &username);
    Product* findProduct(const std::string &name);
    Order* findOrder(const std::string& orderId);
}

#endif