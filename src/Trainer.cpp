//
// Created by eliranbr + msap on 10/11/2021.
//

#include <vector>
#include "../include/Customer.h"
#include "../include/Workout.h"
#include "Trainer.h"

Trainer::Trainer(int t_capacity): capacity(t_capacity), open(0), count(0),customersList(std::vector<Customer*>(t_capacity)),orderList(std::vector<OrderPair>()),salary(0){}
/*** Rule of 5 ***/
Trainer::~Trainer(){ //destructor
    clean();
}
Trainer::Trainer(const Trainer &other):capacity(other.capacity),open(other.open),count(0),customersList(std::vector<Customer*>(other.capacity)),salary(other.salary){
    copy(other);
}
Trainer &Trainer::operator=(const Trainer &other) {
    if(this != &other){
        open = other.open;
        capacity = other.capacity;
        count =  other.count;
        salary = other.salary;
        clean();
        customersList = std::vector<Customer*>(capacity);
        copy(other);
    }

    return *this;
}
Trainer::Trainer(Trainer &&other) :capacity(other.capacity),open(other.open),customersList(std::vector<Customer*>(other.capacity)),count(other.count),salary(other.salary){
    for(Customer* c : other.customersList){
        customersList.push_back(c);
        c = nullptr;
    }
    orderList = std::vector<OrderPair>(other.orderList);
}

Trainer &Trainer::operator=(Trainer &&other) {
    clean();
    open = other.open;
    capacity = other.capacity;
    count =  other.count;
    salary = other.salary;
    for(Customer* c : other.customersList){
        customersList.push_back(c);
        c = nullptr;
    }
    orderList = std::vector<OrderPair>(other.orderList);
    return *this;
}
//helper functions
void Trainer::copy(const Trainer &other){
    for(Customer *c : other.customersList){
        if(c != nullptr) {
            addCustomer(c->clone());
        }
    }
    orderList = std::vector<OrderPair>(other.orderList);
}
void Trainer::clean(){
    for (int i = 0; i < capacity; ++i) {
        if(customersList[i] != nullptr) {
            delete customersList[i];
            customersList[i] = nullptr;
        }
    }
    customersList.clear();
    customersList = std::vector<Customer*>(capacity);
    count = 0;
    orderList.clear();
}
/*** End of Ro5 ***/

int Trainer::getCapacity() const {
    return capacity;
}

void Trainer::addCustomer(Customer* customer) {
    if (!isFull()) {
        customersList[count] = customer;
        count++;
    }
}
void Trainer::removeCustomer(int id){
    bool isFound = false; // found indicator
    for (int i = 0; i < capacity & count > 0; ++i) {
        if(!isFound) {
            if (customersList[i]->getId() == id) { // if found customer, delete it and reduce the list
                isFound = true;
                count--;
                if(i+1 < capacity){
                    customersList[i] = customersList[i+1];
                }
                else{
                    customersList[i] = nullptr;
                }
            }
        } else {
            if(i+1 < capacity){
                customersList[i] = customersList[i+1];
            } else{
                customersList[i] = nullptr;
            }
        }
    }
}
Customer* Trainer::getCustomer(int id){
    for(Customer* c : customersList){
        if(c != nullptr && (*c).getId() == id)
            return c;
    }
    return nullptr;
}
std::vector<Customer*>& Trainer::getCustomers(){
    return customersList;
}
std::vector<OrderPair>& Trainer::getOrders(){
    return orderList;
}
void Trainer::order(const int customer_id, const std::vector<int> workout_ids, const std::vector<Workout>& workout_options){
    for(const int wid : workout_ids){
        orderList.push_back(OrderPair(customer_id,workout_options[wid]));
    }
}

void Trainer::removeOrders(int customerId) {
    std::vector<OrderPair> nOrderList;
    for(OrderPair curr : orderList) {
        if (curr.first != customerId) {
            nOrderList.push_back(curr);
        }
    }
    orderList.clear();
    for(OrderPair curr : nOrderList){
        orderList.push_back(curr);
    }
}
void Trainer::openTrainer(){
    open = 1;
}
void Trainer::closeTrainer(){
    open = 0;
    for(OrderPair op : orderList){
        salary += op.second.getPrice();
    }
    clean();
}
int Trainer::getSalary(){
    return salary;
}
bool Trainer::isOpen(){
    return open;
}

bool Trainer::isFull(){
    return capacity == count;
}

bool Trainer::isEmpty(){
    return count == 0;
}