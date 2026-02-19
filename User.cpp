#include "User.h"
#include "comm.h"
#include "WRFile.h"

#include <iostream>
#include <string>

std::string UserService::promptLine(const std::string& prompt) {
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    return input;
}

User* UserService::login() {
    std::cout << "\n--- Login ---\n";
    std::string username = promptLine("Username: ");
    std::string password = promptLine("Password: ");

    User* user = Common::findUser(username);
    if (!user) {
        std::cout << "User not found.\n";
        return nullptr;
    }
    if (user->password != password) {
        std::cout << "Invalid password.\n";
        return nullptr;
    }

    std::cout << "Login successful. Welcome, " << user->username << ".\n";
    return user;
}

bool UserService::registerUser() {
    std::cout << "\n--- Register ---\n";
    std::string username = promptLine("Choose a username: ");
    if (username.empty()) {
        std::cout << "Username cannot be empty.\n";
        return false;
    }

    if (Common::findUser(username)) {
        std::cout << "Username already exists.\n";
        return false;
    }

    std::string password = promptLine("Enter password: ");
    if (password.empty()) {
        std::cout << "Password cannot be empty.\n";
        return false;
    }
    std::string confirm = promptLine("Confirm password: ");
    if (password != confirm) {
        std::cout << "Passwords do not match.\n";
        return false;
    }
    
    User u;
    u.username = username;
    u.password = password;
    u.role = "customer";
    u.balance = 0.0;
    u.score = 0;

    users.push_back(u);
    wrFileService.saveUsersToFile();

    std::cout << "Registration successful. You can now log in.\n";
    return true;
}

bool UserService::extractOrderId(const std::string& entry, std::string& outId) {
    size_t l = entry.find('[');
    size_t r = entry.find(']');
    if (l == std::string::npos || r == std::string::npos || r <= l + 1) {
        return false;
    }
    outId = entry.substr(l + 1, r - l - 1);
    return !outId.empty();
}

std::string UserService::updateStatusInLine(const std::string& line, const std::string& status) {
    const std::string key = "Status:";
    size_t pos = line.find(key);
    if (pos == std::string::npos) {
        return line + " | Status: " + status;
    }

    std::string prefix = line.substr(0, pos + key.size());
    return prefix + " " + status;
}

void UserService::refreshUserHistoryStatus(User& user) {
    for (auto& entry : user.orderHistory) {
        std::string orderId;
        if (!extractOrderId(entry, orderId)) {
            continue;
        }
        Order* order = Common::findOrder(orderId);
        if (!order) {
            continue;
        }
        entry = updateStatusInLine(entry, order->status);
    }
}