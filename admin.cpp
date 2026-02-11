#include "admin.h"

#include <iostream>
#include <sstream>
#include <iomanip>

void displayProducts(){

}

void displayUsers(){

}

void displayPendingPackages(){

}

void addProduct(){

}

void deleteProduct(){

}

void processNextPackage(){

}

void adminMenu(User &adminUser) {
    while (true) {
        std::cout << "\n=== Admin Menu ===\n"
             << "1. View Products\n"
             << "2. Add Product\n"
             << "3. Delete Product\n"
             << "4. View Users\n"
             << "5. View Pending Packages\n"
             << "6. Process Next Package\n"
             << "7. Logout\n"
             << "Select an option: ";
        std::string choice;
        std::getline(std::cin, choice);
        if (choice == "1") {
            displayProducts();
        } else if (choice == "2") {
            addProduct();
        } else if (choice == "3") {
            deleteProduct();
        } else if (choice == "4") {
            displayUsers();
        } else if (choice == "5") {
            displayPendingPackages();
        } else if (choice == "6") {
            processNextPackage();
        } else if (choice == "7") {
            std::cout << "Logging out...\n";
            break;
        } else {
            std::cout << "Invalid choice.\n";
        }
    }
}