#include "Models.h"

OrderItem::OrderItem()
    : productName(""), quantity(0), unitPrice(0.0) {}

OrderItem::OrderItem(const std::string& name, int qty, double price)
    : productName(name), quantity(qty), unitPrice(price) {}

double OrderItem::lineTotal() const {
    return static_cast<double>(quantity) * unitPrice;
}

// ---------------- Product ----------------

Product::Product()
    : name(""), category(""), price(0.0), stock(0), soldCount(0) {}

Product::Product(const std::string& name, const std::string& category, double price, int stock)
    : name(name), category(category), price(price), stock(stock), soldCount(0) {}

bool Product::isInStock(int qty) const {
    return stock >= qty;
}

void Product::reduceStock(int qty) {
    stock -= qty;
}

void Product::addStock(int qty) {
    stock += qty;
}

void Product::addSold(int qty) {
    soldCount += qty;
}

void Product::setPrice(double p) {
    price = p;
}

void Product::setName(const std::string& n) {
    name = n;
}

void Product::setCategory(const std::string& c) {
    category = c;
}

// ---------------- Order ----------------

Order::Order()
    : id(""), username(""), totalCost(0.0), destinationCity('A'), address(""), status("Pending") {}

int Order::totalItems() const {
    int sum = 0;
    for (const auto& it : items) {
        sum += it.quantity;
    }
    return sum;
}

double Order::subtotal() const {
    double sum = 0.0;
    for (const auto& it : items) {
        sum += it.lineTotal();
    }
    return sum;
}

void Order::recomputeTotal(double shippingCost) {
    totalCost = subtotal() + shippingCost;
}

// ---------------- Package ----------------

Package::Package()
    : id(""), orderId(""), username(""), destinationCity('A'), address(""),
      score(0), status("Pending"), routeDisplay(""), routeDistance(0), enqueueIndex(0) {}

int Package::computeScoreFromItems() const {
    int sum = 0;
    for (const auto& it : items) {
        sum += it.quantity;
    }
    return sum;
}

void Package::updateRoute(const std::string& routeText, int distance) {
    routeDisplay = routeText;
    routeDistance = distance;
}

//------------------ USER ----------------

User::User() {
    username = "";
    password = "";
    role = "customer";
    balance = 0.0;
    score = 0;
}

User::User(const std::string& username, const std::string& password, const std::string& role) {
    this->username = username;
    this->password = password;
    this->role = role;
    this->balance = 0.0;
    this->score = 0;
}
