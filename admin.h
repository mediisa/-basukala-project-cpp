#ifndef ADMIN_H
#define ADMIN_H

#include "Models.h"

class UserService;

class AdminService {
public:
    explicit AdminService(UserService& userService);

    void adminMenu(User& adminUser);

private:
    UserService& userService_;
    bool tryParseInt(const std::string& s, int& out);

    void displayProducts();
    void printCategoryOptions();
    void displayProductsByCategorySorted(const std::string& categoryFilter , bool& sortByPrice);
    void selectProductByCategoryByNumber();
    void displayByTopSellingNamePriceProduct(int choice);

    void displayUsers();

    void addProduct();
    void deleteProduct();
    void editProduct(std::string& name);

    void processNextPackage();
    void displayPendingPackages();
    
};
//extern AdminService adminService;

#endif