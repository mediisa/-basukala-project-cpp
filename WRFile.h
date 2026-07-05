#ifndef WRFILE_H
#define WRFILE_H

#include "comm.h"
#include "Models.h"
#include <vector>

class WRFileService {
public:
    void loadUsersFromFile();
    void saveUsersToFile();

    void loadProductsFromFile();
    void saveProductsToFile();

    void loadOrdersFromFile();
    void saveOrdersToFile();

    void loadPackagesFromFile();
    void savePackagesToFile();

    void loadAll();
    void saveAll();

private:
    void ensureDefaultAdmin();
    static int extractNumericSuffix(const std::string& id);
    std::string encodeOrderItems(const std::vector<OrderItem>& items);
    std::vector<OrderItem> decodeOrderItems(const std::string& encoded);
};

extern WRFileService wrFileService;

#endif