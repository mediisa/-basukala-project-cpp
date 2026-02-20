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
    std::string promptLine(const std::string& prompt);
    bool tryParseInt(const std::string& s, int& out);

    void displayProducts();
    void printCategoryOptions();
    void displayProductsByCategorySorted(const std::string& categoryFilter);
    void selectProductByCategoryByNumber();
    void displayTopSellingProduct();
    void displayByName();
    void displayByPriceProduct();

    void displayUsers();

    void addProduct();
    void deleteProduct();
    void editProduct();

    void processNextPackage();
    void displayPendingPackages();
    
};
//extern AdminService adminService;

#endif