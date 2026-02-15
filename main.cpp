#include "common.h"
#include "admin.h"
#include "customer.h"
#include "WRFile.h"
#include <iostream>


bool registerUser();
User* login();
void mainMenu();
void LFile();

int main() {
    std::cout << "Basukala Project Started!  \n\n ";


    LFile();
    productTrie.build(products);   
    mainMenu();

    
    return 0;
}


bool registerUser() {
    std::cout << "\n--- Registration ---\n";
    std::string username, password, confirm;
    std::cout << "Enter username: ";
    std::getline(std::cin, username);
    if (username.empty()) {
        std::cout << "Username cannot be empty.\n";
        return false;
    }
    if (findUser(username)) {
        std::cout << "Username already exists.\n";
        return false;
    }
    std::cout << "Enter password: ";
    std::getline(std::cin, password);
    std::cout << "Confirm password: ";
    std::getline(std::cin, confirm);
    if (password != confirm) {
        std::cout << "Passwords do not match.\n";
        return false;
    }
    User newUser;
    newUser.username = username;
    newUser.password = password;
    newUser.role = "CUSTOMER";
    newUser.balance = 0.0;
    newUser.score = 0;
    users.push_back(newUser);
    saveUsersToFile();
    std::cout << "Registration successful!\n";
    return true;
}

User* login() {
    std::cout << "\n--- Login ---\n";
    std::string username, password;
    std::cout << "Username: ";
    std::getline(std::cin, username);
    std::cout << "Password: ";
    std::getline(std::cin, password);
    User* user = findUser(username);
    if (!user || user->password != password) {
        std::cout << "Invalid credentials.\n";
        return nullptr;
    }
    std::cout << "Welcome, " << username << "!\n";
    return user;
}

void mainMenu() {
    while (true) {
        std::cout << "\n=== Main Menu ===\n"
             << "1. Register\n"
             << "2. Login\n"
             << "3. Exit\n"
             << "Select an option: ";
        std::string choice;
        std::getline(std::cin, choice);
        if (choice == "1") {
            registerUser();
        } else if (choice == "2") {
            User* user = login();
            if (!user) continue;
            if (user->role == "ADMIN") {
                adminMenu(*user);
            } else {
                customerMenu(*user);
            }
        } else if (choice == "3") {
            std::cout << "Goodbye!\n";
            break;
        } else {
            std::cout << "Invalid option.\n";
        }
    }
}

void LFile(){
    loadUsersFromFile();
    loadProductsFromFile();
    loadOrdersFromFile();
    loadPackagesFromFile(); 
}