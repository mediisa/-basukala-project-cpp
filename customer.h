#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "common.h"
#include <iostream>
#include <vector>

void searchProductsWithSuggestions();
Product* selectProductByName();
void addToCart(std::vector<OrderItem> &cart);
void removeFromCart(std::vector<OrderItem> &cart);
void displayCart(const std::vector<OrderItem> &cart);
void checkout(User &user, std::vector<OrderItem> &cart);
void addFunds(User &user);
void customerMenu(User &user);


char promptDestinationCity();

#endif