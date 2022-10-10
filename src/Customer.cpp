//
// Created by eliranbr + msap on 10/11/2021.
//

#include <vector>
#include <string>
#include "../include/Workout.h"
#include <algorithm>
#include <functional>
#include "Customer.h"

/**** Customer  ****/
Customer::Customer(std::string c_name, int c_id): name(c_name), id(c_id){}
std::string Customer::getName() const{
    return name;
}
int Customer::getId() const{
    return id;
}

/**** Sweaty Customer  ****/
SweatyCustomer::SweatyCustomer(std::string name, int id): Customer(name,id){} // constructor
Customer* SweatyCustomer::clone() {return new SweatyCustomer(*this);}
std::vector<int> SweatyCustomer::order(const std::vector<Workout> &workout_options){
    std::vector<int> prefWO;
    for(Workout w : workout_options){
        if(w.getType()==CARDIO)
            prefWO.push_back(w.getId());
    }
    return prefWO;
}
std::string SweatyCustomer::toString() const{
    return getName() + ",swt";
}


/**** Cheap Customer  ****/
CheapCustomer::CheapCustomer(std::string name, int id):Customer(name,id){}
Customer* CheapCustomer::clone() {return new CheapCustomer(*this);}
std::vector<int> CheapCustomer::order(const std::vector<Workout> &workout_options) {
    std::vector<int> prefWO;
    const Workout *cheapest = &workout_options[0]; // pointer to the first workout
    prefWO.push_back((*cheapest).getId());
    for(const Workout& w : workout_options){
        if(w.getPrice() < (*cheapest).getPrice()) {  // update prefWO and the current cheapest
            prefWO.clear();
            prefWO.push_back(w.getId()); //// check for memory leak
            cheapest = &w;
        }
    }
    return prefWO;
}
std::string CheapCustomer::toString() const{
    return getName() + ",chp";
}

/**** HeavyMuscle Customer  ****/
HeavyMuscleCustomer::HeavyMuscleCustomer(std::string name, int id):Customer(name,id){}
Customer* HeavyMuscleCustomer::clone() {return new HeavyMuscleCustomer(*this);}

std::vector<int> HeavyMuscleCustomer::order(const std::vector<Workout> &workout_options){
    std::vector<int> prefWO;
    std::vector<priceId> sortWO;
    for(Workout w : workout_options){
        if(w.getType() == ANAEROBIC)
            sortWO.push_back(priceId(w.getPrice(),w.getId()));
    }
    std::sort(sortWO.begin(),sortWO.end(),std::greater<priceId>());
    for(priceId pair: sortWO)
        prefWO.push_back(pair.second);
    return prefWO;
}
std::string HeavyMuscleCustomer::toString() const{
    return getName() + ",mcl";
}

/**** FullBody Customer  ****/
FullBodyCustomer::FullBodyCustomer(std::string name, int id):Customer(name,id){}
Customer* FullBodyCustomer::clone() {return new FullBodyCustomer(*this);}

std::vector<int> FullBodyCustomer::order(const std::vector<Workout> &workout_options){
    std::vector<int> prefWO;
    const Workout *cheapCar = nullptr;
    const Workout *expenMix = nullptr;
    const Workout *cheapAna = nullptr;
    for(const Workout& w : workout_options){
        switch (w.getType()) {
            case CARDIO:
                if(cheapCar == nullptr)
                    cheapCar = &w;
                else if((*cheapCar).getPrice() > w.getPrice())
                    cheapCar = &w;
                break;
            case MIXED:
                if(expenMix == nullptr)
                    expenMix = &w;
                else if((*expenMix).getPrice() < w.getPrice())
                    expenMix = &w;
                break;
            case ANAEROBIC:
                if(cheapAna == nullptr)
                    cheapAna = &w;
                else if((*cheapAna).getPrice() > w.getPrice())
                    cheapAna = &w;
                break;
            default:
                break;
        }
    }
    prefWO.push_back((*cheapCar).getId());
    prefWO.push_back((*expenMix).getId());
    prefWO.push_back((*cheapAna).getId());
    return prefWO;
}
std::string FullBodyCustomer::toString() const{
    return getName() + ",fbd";
}
