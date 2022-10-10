//
// Created by eliranbr + msap on 10/11/2021.
//
#include "../include/Trainer.h"
#include "../include/Action.h"
#include "Studio.h"
#include <fstream>
#include <iostream>
#include <sstream>

Studio::Studio():open(0),cCounter(0){}
Studio::Studio(const std::string &configFilePath):open(0),cCounter(0){
    //read file
    std::ifstream infile(configFilePath);
    std::string line;
    int parameter = 0; // indicator to which type of parameter we are receiving
    int tCount = 0; // trainer count
    while(std::getline(infile,line)){ // iterate through the lines
        if(!(line[0] == '#' || line =="")){  // ignore irrelevant lines
            if(parameter == 0){ // get the number of trainers
                tCount = std::stoi(line);
                parameter++;
            } else if( parameter == 1){ // create trainers
                for (int i = 0; i < tCount; ++i) {
                    trainers.push_back(new Trainer(std::stoi(line.substr(0,line.find(",")))));
                    line.erase(0, line.find(",")+1);
                }
                parameter++;
            } else if(parameter == 2){ // create workouts
                std::string name = line.substr(0, line.find(", ")); // get name
                line.erase(0, line.find(", ") + 2); //erase name
                WorkoutType wt;
                std::string temp = line.substr(0,line.find(", ")); // get type
                if(temp == "Anaerobic") {wt = ANAEROBIC;}
                else if (temp == "Mixed") {wt = MIXED;}
                else {wt = CARDIO;}
                line.erase(0, line.find(", ") + 2); // erase type
                int price = std::stoi(line); // get price
                workout_options.emplace_back(Workout(workout_options.size(),name,price,wt));
            }
        }
    }
}
/*** Rule of 5 ***/
Studio::~Studio(){ //destructor
    Clean();
}
Studio::Studio(const Studio &other):open(other.open),cCounter(other.cCounter),workout_options(other.workout_options) {
    for(Trainer *t : other.trainers) {
        trainers.emplace_back(new Trainer(*t));
    }
    for(BaseAction *ba : other.actionsLog){
        actionsLog.emplace_back(ba->clone());
    }
}
Studio& Studio::operator=(const Studio &other) { // copy assignment operator
    if(this != &other){
        for (unsigned i = 0; i < actionsLog.size(); ++i) {
            delete actionsLog[i];
        }
        open = other.open;
        for (int i = 0; i < other.getNumOfTrainers(); ++i) {
            if (other.trainers[i] != nullptr) {
                delete trainers[i];
                trainers[i] = new Trainer(*other.trainers[i]);
            }
        }
        actionsLog.clear();
        for(BaseAction *ba : other.actionsLog){
            actionsLog.emplace_back(ba->clone());
        }
        workout_options = std::vector<Workout>(other.workout_options);
        cCounter = other.cCounter;
    }
    return *this;
}

Studio::Studio(Studio &&other):open(other.open),cCounter(other.cCounter),workout_options(other.workout_options) {
    std::swap(trainers,other.trainers);
    std::swap(actionsLog,other.actionsLog);
}
Studio& Studio::operator=(Studio &&other) { // move assignment operator
    if(this != &other){
        open = other.open;
        std::swap(trainers,other.trainers);
        std::swap(actionsLog, other.actionsLog);
        workout_options = std::vector<Workout>(other.workout_options);
        cCounter = other.cCounter;
        other.Clean();
    }
}
/*** End of Ro5 ***/

void Studio::start() {
    open = 1;
    std::cout << "Studio is now open!" << std::endl;
    std::string input = "";
    while (std::getline(std::cin, input) && open) { //open 0 Shalom,swt Dan,chp Bob,fbd
        std::istringstream iss(input); // makes it so we get a line
        std::string command = input.substr(0, input.find(' '));
        input.erase(0, input.find(' ') + 1); //remove the command from the input
        if (command == "open") { // OpenTrainer

            int id = std::stoi(input.substr(0, input.find(' ')));
            std::vector<Customer *> customers;
            input.erase(0, input.find(' ') + 1);

            while (input != "") { // create all customers
                std::string name = input.substr(0, input.find(','));
                input.erase(0, input.find(',') + 1);
                std::string type = input.substr(0, input.find(' '));
                input.erase(0, 4);
                if (type == "swt") {
                    customers.push_back(new SweatyCustomer(name, cCounter));
                } else if (type == "chp") {
                    customers.push_back(new CheapCustomer(name, cCounter));
                } else if (type == "mcl") {
                    customers.push_back(new HeavyMuscleCustomer(name, cCounter));
                } else if (type == "fbd"){
                    customers.push_back(new FullBodyCustomer(name, cCounter));
                }
                cCounter++;
            }
            OpenTrainer *ot = new OpenTrainer(id, customers);
            ot->act(*this);
            actionsLog.push_back(ot);
        }
        if (command == "order"){ //order
            int id = std::stoi(input.substr(0, input.find(' '))); // get Trainers Id
            Order *order = new Order(id); // Create Order Action
            order->act(*this);
            actionsLog.push_back(order);
        }
        if (command == "move"){ // MoveCustomer
            int o_id = std::stoi(input.substr(0, input.find(' ')));
            input.erase(0, input.find(' ') + 1);
            int d_id = std::stoi(input.substr(0, input.find(' ')));
            input.erase(0, input.find(' ') + 1);
            int c_id = std::stoi(input.substr(0, input.find(' ')));
            input.erase(0, input.find(' ') + 1);
            MoveCustomer *move = new MoveCustomer(o_id,d_id,c_id);
            move->act(*this);
            actionsLog.push_back(move);
        }
        if (command == "close"){ //close
            int t_id = std::stoi(input.substr(0, input.find(' ')));
            Close *cl = new Close(t_id);
            cl->act(*this);
            actionsLog.push_back(cl);
        }
        if (command == "closeall"){ //CloseAll
            auto *ca = new CloseAll();
            ca->act(*this);
            actionsLog.emplace_back(ca);
            open = 0;
            break;
        }
        if (command == "workout_options"){// workout options
            PrintWorkoutOptions *wo = new PrintWorkoutOptions();
            actionsLog.push_back(wo);
            wo->act(*this);
        }
        if (command == "status"){ //PrintTrainerStatus
            int t_id = std::stoi(input.substr(0, input.find(' ')));
            auto *pts = new PrintTrainerStatus(t_id);
            pts->act(*this);
            actionsLog.emplace_back(pts);
        }
        if (command == "log"){ // PrintActionsLog
            auto *pa = new PrintActionsLog();
            pa->act(*this);
            actionsLog.emplace_back(pa);
        }
        if (command == "backup"){//BackupStudio
            auto *bu = new BackupStudio();
            actionsLog.emplace_back(bu);
            bu->act(*this);
        }
        if (command == "restore"){//restoreStudio
            auto *rs = new RestoreStudio();
            rs->act(*this);
            actionsLog.emplace_back(rs);
        }
        if( command == "test"){
            Studio st(std::move(*backup));
            std::cout<<"test"<<std::endl;
            //delete &st;
        }
    }
}
int Studio::getNumOfTrainers() const{
    return trainers.size();
}
Trainer* Studio::getTrainer(int tid){
    if(trainers.size() <= tid || tid < 0)
        return nullptr;
    return trainers[tid];
}
const std::vector<BaseAction*>& Studio::getActionsLog() const{
    return actionsLog;
} // Return a reference to the history of actions
std::vector<Workout>& Studio::getWorkoutOptions(){
    return workout_options;
}
void Studio::Clean() {
    for (unsigned i = 0; i < trainers.size(); ++i) {
        if(trainers[i] != nullptr){
            //trainers[i]->clean();
            delete trainers[i];
        }
    }
    for (unsigned i = 0; i < actionsLog.size(); ++i) {
        delete actionsLog[i];
    }
    cCounter = 0;
    actionsLog.clear();
    workout_options.clear();
    trainers.clear();
    open = false;
}