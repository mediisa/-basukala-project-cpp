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
  
    bool found = false;

    for(auto& user : users){
        if (user.getusername() != username) continue;
        
        found = true;
        std::string inputHash = simpleHash(password + user.getsalt());

        if (user.getpasswordHash() == inputHash) {
            std::cout << "Login successful. Welcome, " << user.getusername() << ".\n";
            return &user;
        }
    }

    if(found){
        std::cout << "Invalid password.\n";
    }else{
        std::cout << "User not found.\n";
    }
    return nullptr;
}

bool UserService::registerUser() {
    std::cout << "\n--- Register ---\n";
    std::string username = Common::promptLine("Choose a username: ");
    if (username.empty()) {
        std::cout << "Username cannot be empty.\n";
        return false;
    }


    std::string password = Common::promptLine("Enter password: ");
    if (password.empty()) {
        std::cout << "Password cannot be empty.\n";
        return false;
    }

    for(const auto& user : users ){
        if(user.getusername() == username && user.getpassword() == password){
            std::cout << "This username and password already exist.\n";
            return false;
        }
    }
    std::string confirm = Common::promptLine("Confirm password: ");
    if (password != confirm) {
        std::cout << "Passwords do not match.\n";
        return false;
    }
    std::string salt = generateSalt();
    std::string hash = simpleHash(password + salt);
    User u (username , salt , password , hash , "customer");

    users.push_back(u);
    wrFileService.saveUsersToFile();

    std::cout << "Registration successful. You can now log in.\n";
    return true;
}