#include "User.h"
#include "comm.h"
#include "WRFile.h"

#include <iostream>
#include <string>
#include <functional>
#include <random>

std::string UserService::simpleHash(const std::string& input) {
    return std::to_string(std::hash<std::string>{}(input));
}

std::string UserService::generateSalt(size_t length ) {
    static const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

    std::string salt;
    salt.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        salt.push_back(charset[dist(gen)]);
    }
    return salt;
}

User* UserService::login() {
    std::cout << "\n--- Login ---\n";
    std::string username = Common::promptLine("Username: ");
    std::string password = Common::promptLine("Password: ");

    User* user = Common::findUser(username);
    if (!user) {
        std::cout << "User not found.\n";
        return nullptr;
    }
  
    std::string inputHash = simpleHash(password + user->getsalt());

    if (user->getpasswordHash() != inputHash) {
        std::cout << "Invalid password.\n";
        return nullptr;
    }

    std::cout << "Login successful. Welcome, " << user->getusername() << ".\n";
    return user;
}

bool UserService::registerUser() {
    std::cout << "\n--- Register ---\n";
    std::string username = Common::promptLine("Choose a username: ");
    if (username.empty()) {
        std::cout << "Username cannot be empty.\n";
        return false;
    }

    if (Common::findUser(username)) {
        std::cout << "Username already exists.\n";
        return false;
    }

    std::string password = Common::promptLine("Enter password: ");
    if (password.empty()) {
        std::cout << "Password cannot be empty.\n";
        return false;
    }
    std::string confirm = Common::promptLine("Confirm password: ");
    if (password != confirm) {
        std::cout << "Passwords do not match.\n";
        return false;
    }
    std::string salt = generateSalt();
    std::string hash = simpleHash(password + salt);
    User u (username , salt , password , hash , "customer");
    //User u (username , password , "customer");
    /*u.username = username;
    u.password = password;
    u.role = "customer";
    u.balance = 0.0;
    u.score = 0;
*/
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
    for (auto& entry : user.getorderHistory()) {
        std::string orderId;
        if (!extractOrderId(entry, orderId)) {
            continue;
        }
        Order* order = Common::findOrder(orderId);
        if (!order) {
            continue;
        }
        entry = updateStatusInLine(entry, order->getstatus());
    }
}