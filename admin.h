#ifndef ADMIN_H
#define ADMIN_H

#include "common.h"


void displayProducts();
void displayUsers();
void displayPendingPackages();

void addProduct();
void deleteProduct();

void processNextPackage();


void adminMenu(User &adminUser);

#endif
