#include "Models.h"

//--------------- Product ----------------

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

std::string Product::getname() const {
    return name;
}
std::string Product::getcategory() const{
    return category;
}
double Product::getprice() const{
    return price;
}
int Product::getstock() const{
    return stock;
}
int Product::getsoldCount() const{
    return soldCount;
}


// ---------------- Order ----------------

Order::Order(const std::string& id , const std::string& username ,  const std::string& passwordHash, const std::vector<OrderItem>& items , 
            const double& totalCost , const char& destinationCity , const std::string& address , const std::string& status)
            : id(id),
            username(username),
            password(passwordHash),
            items(items),
            totalCost(totalCost),
            destinationCity(destinationCity),
            address(address),
            status(status) {}

Order::Order(const std::string& id , const std::string& username , const std::string& passwordHash ,const std::vector<OrderItem>& items ,
             const char& destinationCity , const std::string& address , const std::string& status)
            : id(id),
            username(username),
            password(passwordHash),
            items(items),
            destinationCity(destinationCity),
            address(address),
            status(status) {}

std::string Order::getid() const{
    return id;
}
std::string Order::getusername() const{
    return username;
}
std::string Order::getpassworHash() const{
    return password;
}
std::vector<OrderItem> Order::getitems() const{
    return items;
}
double Order::gettotalCost() const{
    return totalCost;    
}
char Order::getdestinationCity() const{
    return destinationCity;
}
std::string Order::getaddress() const{
    return address;
}
std::string Order::getstatus() const{
    return status;
}
void Order::settotalCost(double newtotalCost) {
    totalCost = newtotalCost;
}
void Order::setStatus(std::string newstatus){
    status = newstatus;
}
// ---------------- Package ----------------

Package::Package()
    : id(""), orderId(""), username(""), passwordHash(""), destinationCity('A'), address(""),
      score(0), status("Pending"), routeDisplay(""), routeDistance(0), enqueueIndex(0) {}

Package::Package(const std::string& id , const std::string& orderId , const std::string& username , const std::string& passwordHash ,
                const std::vector<OrderItem>& items , const char& destinationCity , const std::string& address ,
                const std::string& status , const std::string& routeDisplay){
                    this->id = id;
                    this->orderId = orderId;
                    this->username = username;
                    this->passwordHash = passwordHash;
                    this->items = items;
                    this->destinationCity = destinationCity;
                    this->address = address;
                    this->status = status;
                    this->routeDisplay = routeDisplay;
                }

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

std::string Package::getid() const{
    return id;
}
std::string Package::getorderId() const{
    return orderId;
}
std::string Package::getusername() const{
    return username;
}
std::string Package::getpasswordHash() const{
    return passwordHash;
}
std::vector<OrderItem> Package::getitems() const{
    return items;
}
char Package::getdestinationCity() const{
    return destinationCity;
}
std::string Package::getaddress() const{
    return address;
}
int Package::getscore() const{
    return score;
}
std::string Package::getstatus() const{
    return status;
}
std::string Package::getrouteDisplay() const{
    return routeDisplay;
}
int Package::getrouteDistance() const{
    return routeDistance;
}
long long Package::getenqueueIndex() const{
    return enqueueIndex;
}

void Package::setstatus(std::string newstatus){
    status = newstatus;
}
void Package::setrouteDistance(int newrouteDistance){
    routeDistance = newrouteDistance;
}
void Package::setenqueueIndex(long long newenqueueIndex){
    enqueueIndex = newenqueueIndex;
}
void Package::setrouteDisplay(std::string newrouteDisplay){
    routeDisplay = newrouteDisplay;
}
void Package::setscore(int newscore){
    score = newscore;
}
//------------------ USER ----------------

User::User() {
    username = "";
    password = "";
    role = "customer";
    balance = 0.0;
    score = 0;
}

User::User(const std::string& username, const std::string& password, const std::string& passwordHash, const std::string& role) {
    this->username = username;
    this->password = password;
    this->passwordHash = passwordHash;
    this->salt = "";
    this->role = role;
    this->balance = 0.0;
    this->score = 0;
}

User::User(const std::string& username, const std::string& salt, const std::string& password, const std::string& passwordHash, const std::string& role) {
    this->username = username;
    this->password = password;
    this->passwordHash = passwordHash;
    this->salt = salt;
    this->role = role;
    this->balance = 0.0;
    this->score = 0;  
}

std::string User::getusername() const{
    return username;
}
std::string User::getpassword() const{
    return password;
}
std::string User::getpasswordHash() const{
    return passwordHash;
}
std::string User::getsalt() const{
    return salt;
}
std::string User::getrole() const{
    return role;
}
double User::getbalance() const{
    return balance;
}
int User::getscore() const{
    return score;
}
std::vector<std::string> User::getorderHistory() const{
    return orderHistory;
}
void User::reduceBlance(int qty) {
    balance -= qty;
}
void User::addBlance(int qty) {
    balance += qty;
}
void User::addScore(int qty) {
    score += qty;
}

