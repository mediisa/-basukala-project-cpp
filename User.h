#ifndef USER_H
#define USER_H

#include "Models.h"

class UserService {
public:
    User* login();
    bool registerUser();
    void refreshUserHistoryStatus(User& user);
    std::string simpleHash(const std::string& input);
    std::string generateSalt(size_t length = 8);
private:
    bool extractOrderId(const std::string& entry, std::string& outId);
    std::string updateStatusInLine(const std::string& line, const std::string& status);
};

#endif
