#ifndef USER_H
#define USER_H

#include "Models.h"

class UserService {
public:
    User* login();
    bool registerUser();
    void refreshUserHistoryStatus(User& user);

private:
    std::string promptLine(const std::string& prompt);
    bool extractOrderId(const std::string& entry, std::string& outId);
    std::string updateStatusInLine(const std::string& line, const std::string& status);
};

#endif
