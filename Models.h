#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <vector>

struct OrderItem {
    std::string productName;
    int quantity;
    double unitPrice;

OrderItem()
    : productName(""), quantity(0), unitPrice(0.0) {}

OrderItem(const std::string& name, int qty, double price)
    : productName(name), quantity(qty), unitPrice(price) {}   
};

class Product {
private:
    std::string name;
    std::string category;
    double price;
    int stock;
    int soldCount;
public:
    Product();
    Product(const std::string& name, const std::string& category, double price, int stock);

    bool isInStock(int qty) const;
    void reduceStock(int qty);
    void addStock(int qty);
    void addSold(int qty);

    void setPrice(double p);
    void setName(const std::string& n);
    void setCategory(const std::string& c);

    std::string getname() const;
    std::string getcategory() const;
    double getprice() const;
    int getstock() const;
    int getsoldCount() const;

};

class Order {
private:
    std::string id;
    std::string username;
    std::string password;
    std::vector<OrderItem> items;
    double totalCost;
    char destinationCity;
    std::string address;
    std::string status;
public:
   Order(const std::string& id , const std::string& username ,  const std::string& passwordHash ,const std::vector<OrderItem>& items , 
           const double& totalCost , const char& destinationCity , const std::string& address , const std::string& status);

    Order(const std::string& id , const std::string& username , const std::string& passwordHash , const std::vector<OrderItem>& items , 
            const char& destinationCity , const std::string& address , const std::string& status);

std::string getid() const;
std::string getusername() const;
std::string getpassworHash() const;
std::vector<OrderItem> getitems() const;
double gettotalCost() const;
char getdestinationCity() const;
std::string getaddress() const;
std::string getstatus() const;
void settotalCost(double newtotalCost) ;
void setStatus(std::string newstatus);

};

class Package {
private:
    std::string id;
    std::string orderId;
    std::string username;
    std::string passwordHash;
    std::vector<OrderItem> items;
    char destinationCity;
    std::string address;
    int score;
    std::string status;
    std::string routeDisplay;
    int routeDistance;
    long long enqueueIndex;
public:
    Package();
    Package(const std::string& id , const std::string& orderId , const std::string& username , const std::string& passwordHash,
            const std::vector<OrderItem>& items , const char& destinationCity , const std::string& address ,
            const std::string& status , const std::string& routeDisplay);
    int computeScoreFromItems() const;
    void updateRoute(const std::string& routeText, int distance);
    
    std::string getid() const;
    std::string getorderId() const;
    std::string getusername() const;
    std::string getpasswordHash() const;
    std::vector<OrderItem> getitems() const;
    char getdestinationCity() const;
    std::string getaddress() const;
    int getscore() const;
    std::string getstatus() const;
    std::string getrouteDisplay() const;
    int getrouteDistance() const;
    long long getenqueueIndex() const;
    void setstatus(std::string newstatus);
    void setrouteDistance(int newrouteDistance);
    void setenqueueIndex(long long newenqueueIndex);
    void setrouteDisplay(std::string routeDisplay);
    void setscore(int newscore);
    
};

class User {
private:
    std::string username;
    std::string password;
    std::string passwordHash;   
    std::string salt;
    std::string role;
    double balance;
    int score;
    std::vector<std::string> orderHistory;
public:
    User();
    User(const std::string& username, const std::string& password, const std::string& passwordHash, const std::string& role);
    User(const std::string& username, const std::string& salt, const std::string& password, const std::string& passwordHash, const std::string& role);

    std::string getusername() const;
    std::string getpassword() const;
    std::string getpasswordHash() const;
    std::string getsalt() const;
    std::string getrole() const;
    double getbalance() const;
    int getscore() const;
    std::vector<std::string> getorderHistory() const;

    void reduceBlance(int qty);
    void addBlance(int qty);
    void addScore(int qty);

};

#endif