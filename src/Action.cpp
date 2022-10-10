//
// Created by eliranbr + msap on 10/11/2021.
//

#include <string>
#include <iostream>
#include "Action.h"
#include "Studio.h"

/**** BaseAction ****/
BaseAction::BaseAction(){};
ActionStatus BaseAction::getStatus() const{
    return status;
}
void BaseAction::complete(){
    status = COMPLETED;
}
void BaseAction::error(std::string errorMsg){
    status = ERROR;
    this->errorMsg = errorMsg;
    std::cout<<errorMsg<<std::endl;
}
std::string BaseAction::getErrorMsg() const{
    return errorMsg;
}

/**** OpenTrainer ****/
OpenTrainer::OpenTrainer(int id, std::vector<Customer *> &customersList): trainerId(id),customers(customersList){}

BaseAction *OpenTrainer::clone() {return new OpenTrainer(*this);}
void OpenTrainer::act(Studio &studio) {
    input = "open " + std::to_string(trainerId);
    Trainer* trainer = studio.getTrainer(trainerId);
    if(trainer != nullptr && !trainer->isOpen()) {
        trainer->openTrainer();
        for (int i = 0; i < customers.size(); ++i) {// insert customers to trainer
            if(!trainer->isFull()) { // make sure we do not go over the capacity
                trainer->addCustomer(customers[i]);
                input += " " +(*customers[i]).toString();
            } else{
                studio.cCounter--; //update id for customers who did not have enough space to enter the session
                input += " " +(*customers[i]).toString();
                delete customers[i];
            }
        }
        complete();
    } else { //rollback id's since error has occurred.
        for (int i = 0; i < customers.size(); ++i) {
            studio.cCounter--;
            input += " " +(*customers[i]).toString();
            delete customers[i];
        }
        error( "Error: Trainer does not exist or is already open");
    }
    customers.clear();
}
std::string OpenTrainer::toString() const {

    if(getStatus() == 0){
        return input + " Completed";
    }
    return input + " " + getErrorMsg();
}

/**** PrintWorkoutOptions ****/
PrintWorkoutOptions::PrintWorkoutOptions() {}
BaseAction *PrintWorkoutOptions::clone() {return new PrintWorkoutOptions(*this);}
void PrintWorkoutOptions::act(Studio &studio) {
    const std::vector<Workout> &wo = studio.getWorkoutOptions();
    for (const Workout &w: wo) {
        switch(w.getType()){
            case 0:
                std::cout <<w.getName() + ", " +  "Anaerobic" + ", " + std::to_string(w.getPrice()) <<std::endl;
                break;
            case 1:
                std::cout <<w.getName() + ", " +  "Mixed" + ", " + std::to_string(w.getPrice()) <<std::endl;
                break;
            case 2:
                std::cout <<w.getName() + ", " +  "Cardio" + ", " + std::to_string(w.getPrice()) <<std::endl;
                break;
        }
    }
    complete();
}
std::string PrintWorkoutOptions::toString() const {
    return "PrintWorkoutOptions Completed";
}

/**** Order ****/
Order::Order(int id):trainerId(id){}
BaseAction *Order::clone() {return new Order(*this);}
void Order::act(Studio &studio){
    Trainer* trainer = studio.getTrainer(trainerId);
    if(trainer != nullptr && trainer->isOpen()){
        for(Customer* c : trainer->getCustomers()){
            if(c != nullptr) {
                std::vector<int> workouts = c->order(studio.getWorkoutOptions());
                trainer->order(c->getId(), workouts, studio.getWorkoutOptions());
                for(int wid : workouts){
                    std::cout<< c->getName() + " Is doing " + studio.getWorkoutOptions()[wid].getName()<<std::endl;
                }
            }
        }
        complete();
    } else {
        error("Error: Trainer does not exists or is not Open");
    }
}
std::string Order::toString() const{
    if(getStatus() == COMPLETED){
        return "order " + std::to_string(trainerId) + " Completed";
    }
    else{
        return "order " + std::to_string(trainerId) +" " + getErrorMsg();
    }
}

/**** MoveCustomer ****/
MoveCustomer::MoveCustomer(int src, int dst, int customerId):srcTrainer(src),dstTrainer(dst),id(customerId) {}
BaseAction *MoveCustomer::clone() {return new MoveCustomer(*this);}

void MoveCustomer::act(Studio &studio) {
    Trainer *o_trainer = studio.getTrainer(srcTrainer);
    Trainer *d_trainer = studio.getTrainer(dstTrainer);
    if ((o_trainer != nullptr && o_trainer->isOpen()) && (d_trainer != nullptr && d_trainer->isOpen())) {
        Customer *customer = o_trainer->getCustomer(id);
        if (customer != nullptr && !d_trainer->isFull()) {
            o_trainer->removeCustomer(id);
            o_trainer->removeOrders(id);
            d_trainer->addCustomer(customer);
            d_trainer->order(id, customer->order(studio.getWorkoutOptions()), studio.getWorkoutOptions());
           if (o_trainer->isEmpty()) {
                o_trainer->closeTrainer();
            }
            complete();
        } else {
            error("Error: Cannot move customer");
        }
    }else {
        error("Error: Cannot move customer");
    }
}
std::string MoveCustomer::toString() const {
    if(getStatus() == COMPLETED){
        return "move " + std::to_string(srcTrainer) + " " + std::to_string(dstTrainer) + " " + std::to_string(id) + " Completed";
    }
    else{
        return "move " + std::to_string(srcTrainer) + " " + std::to_string(dstTrainer) + " " + std::to_string(id) + " " +getErrorMsg();
    }
}

/**** Close ****/
Close::Close(int id):trainerId(id){}
BaseAction *Close::clone() {return new Close(*this);}

void Close::act(Studio &studio) {
    Trainer *t = studio.getTrainer(trainerId);
    if(t != nullptr && t->isOpen()){
        t->closeTrainer();
        complete();
        std::cout<<"Trainer " + std::to_string(trainerId) + " closed. Salary " + std::to_string(t->getSalary()) + "NIS"<<std::endl;
    }else{
        error("Error: Trainer does not exist or is not Open");
    }
}
std::string Close::toString() const {
    if(getStatus() == COMPLETED){
        return "close " + std::to_string(trainerId) + " Completed";
    } else{
        return "close " + std::to_string(trainerId) + " " + getErrorMsg();
    }
}
/**** PrintTrainerStatus ****/
PrintTrainerStatus::PrintTrainerStatus(int id):trainerId(id) {}
BaseAction *PrintTrainerStatus::clone() {return new PrintTrainerStatus(*this);}
void PrintTrainerStatus::act(Studio &studio) {
    Trainer *t = studio.getTrainer(trainerId);
    if(t != nullptr){
        std::cout<<"Trainer " + std::to_string(trainerId) + " Status: ";
        if(t->isOpen()){
            std::cout << "open"<< std::endl;
            if(t->getOrders().size() != 0) {
                std::cout << "Customers:" << std::endl;
                for (Customer *c: t->getCustomers()) {
                    if(c!= nullptr) {
                        std::cout << std::to_string(c->getId()) + " " + c->getName() << std::endl;
                    }
                }
                std::cout << "Orders:" << std::endl;
                int expected = 0;
                for (OrderPair op: t->getOrders()) {
                    std::cout << op.second.getName() + " " + std::to_string(op.second.getPrice()) + "NIS " + std::to_string(op.first)<<std::endl;
                    expected += op.second.getPrice();
                }
                std::cout<< "Current Trainer's salary: " + std::to_string(t->getSalary() + expected ) + "NIS"<<std::endl;
            }
        }else{
            std::cout << "closed"<< std::endl;
        }
    } else{
        std::cout<< "Trainer does not Exist"<<std::endl;
    }
    complete();
}
std::string PrintTrainerStatus::toString() const {
    return "status " + std::to_string(trainerId) + " Completed";
}
/**** PrintActionsLog ****/
PrintActionsLog::PrintActionsLog(){}
BaseAction *PrintActionsLog::clone() {return new PrintActionsLog(*this);}

void PrintActionsLog::act(Studio &studio) {
    for(BaseAction *ba : studio.getActionsLog()){
        std::cout<< ba->toString() <<std::endl;
    }
}
std::string PrintActionsLog::toString() const {
    return "log Completed";
}

/**** Closeall ****/
CloseAll::CloseAll() {}
BaseAction *CloseAll::clone() {return new CloseAll(*this);}

void CloseAll::act(Studio &studio) {
    for (int i = 0; i < studio.getNumOfTrainers(); ++i) {
        if(studio.getTrainer(i)->isOpen()){
            Close cl = Close(i);
            cl.act(studio);
        }
    }
    complete();
}
std::string CloseAll::toString() const{
    return "closeall Completed";
}
/**** BackupStudio ****/
BackupStudio::BackupStudio(){}
BaseAction *BackupStudio::clone(){return new BackupStudio(*this);}
void BackupStudio::act(Studio &studio) {
    if(backup != nullptr) {
        delete backup;
    }
    backup = new Studio(studio);
    complete();
}
std::string BackupStudio::toString() const {return "backup Completed";}

/**** RestoreStudio ****/
RestoreStudio::RestoreStudio(){}
BaseAction *RestoreStudio::clone(){return new RestoreStudio(*this);}
void RestoreStudio::act(Studio &studio) {
    if(backup == nullptr){
        error("No backup available");
    }else {
        studio = *backup;
        complete();
    }
}
std::string  RestoreStudio::toString() const {
    if(getStatus() == ERROR)
        return "restore " + getErrorMsg();
    return "restore Complete";
}

