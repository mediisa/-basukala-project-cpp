#include "comm.h"
#include "WRFile.h"
#include "Admin.h"
#include "Customer.h"
#include "User.h"
#include "ProductTrie.h"
#include "ProductBST.h"
#include <iostream>
#include <string>

static void initializeData();
static void mainMenu();


static UserService userService;
static AdminService    adminService(userService);
static CustomerService customerService(userService);


int main() {
    std::cout << "Basukala Project Started!\n\n";

    initializeData();
    mainMenu();

    return 0;
}

static void initializeData() {
    wrFileService.loadAll();
    productTrie.build(productCatalog.toVector());
}

static void mainMenu() {
    while (true) {
        std::cout << "\n=== Main Menu ===\n"
                  << "1. Register\n"
                  << "2. Login\n"
                  << "3. Exit\n"
                  << "Select an option: ";

        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1") {
            userService.registerUser();
        } else if (choice == "2") {
            User* user = userService.login();
            if (!user) continue;

           /// userService.refreshUserHistoryStatus(*user);

            if (Common::equalsIgnoreCase(user->getrole(), "ADMIN")) {
                adminService.adminMenu(*user);
            } else {
                customerService.customerMenu(*user);
            }
        } else if (choice == "3") {
            wrFileService.saveAll();
            std::cout << "Goodbye!\n";
            break;
        } else {
            std::cout << "Invalid option.\n";
        }
    }
}
