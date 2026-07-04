#ifndef USER_H
#define USER_H

#include "Models.h"

class UserService {
public:
    User* login();
    bool registerUser();
    std::string simpleHash(const std::string& input);
    std::string generateSalt(size_t length = 8);
};

#endif
