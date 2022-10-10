//
// Created by eliranbr + msap on 10/11/2021.
//

#include <string>
#include "Workout.h"

Workout::Workout(int w_id, std::string w_name, int w_price, WorkoutType w_type) : id(w_id), name(w_name), price(w_price),type(w_type){} //constructor
int Workout::getId() const{ // returns ID
    return id;
}
std::string Workout::getName() const{ // returns name
    return name;
}
int Workout::getPrice() const{ // returns price
    return price;
}
WorkoutType Workout::getType() const { // returns type
    return type;
}