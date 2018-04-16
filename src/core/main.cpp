#include <iostream>
#include <vector>
#include <time.h>
#include <stdlib.h>

#include "options.hpp"
#include "utils.hpp"
#include "electrical_circuit.hpp"
#include "solver_soplex.hpp"
#include "solver_armadillo.hpp"
#include "solver_iter_petsc.hpp"
#include "local_search_engine.hpp"
#include "pricing_manager.hpp"
#include "accepter.hpp"


using namespace std;

vector<ElectricalCircuit::NodeID> parseFocalsArgument(string list) {
    vector<ElectricalCircuit::NodeID> result;
    std::string token;
    std::istringstream tokenStream(list);
    while (std::getline(tokenStream, token, ',')) {
       result.push_back((ElectricalCircuit::NodeID)stoi(token));
    }
    return result;
}

AvailableSolvers parseSolverName(string name) {
    if (name == "soplex")
        return kSoplex;
    if (name == "armadillo")
        return kArmadillo;
    if (name == "petsc")
        return kPetSc;

    cerr<<"Unrecognised solver name \""<<name<<"\", defaulting to PetSc"<<endl;

    return kPetSc;
}

int main(int argc, char* argv[]) {
    
    std::cout<< "=== Kea Landscape Tool ==="<<std::endl;
    srand(time(NULL));

    //Read command line arguments

    vector<ElectricalCircuit::NodeID> focals; //Pairwise
    vector<pair<ElectricalCircuit::NodeID,ElectricalCircuit::NodeID> > focal_pairs;
    string network_file = std::string(argv[1]);
    KeaOptions options;

    for (int i = 2; i < argc; i++) {
        string arg = string(argv[i]);

        if (arg == "--focals" || arg == "-f") {
            focals = parseFocalsArgument(string(argv[i+1]));
            i++; //Skip one
        } else if (arg == "--solver" || arg == "-s") {
            options.solver = parseSolverName(string(argv[i+1]));
            i++;
        } else if (arg == "--time" || arg == "-t") {
            options.time_limit = stof(string(argv[i+1]));
            i++;
        } else if (arg == "--iterations" || arg == "-i") {
            options.max_iterations = stoi(string(argv[i+1]));
            i++;
        } else if (arg == "--budget" || arg == "-b") {
            options.budget = stoi(string(argv[i+1]));
            i++;
        } else if (arg == "--d-rate" || arg == "-dr") {
            options.destruction_rate = stoi(string(argv[i+1]));
            i++;
        } else if (arg == "--out" || arg == "-o") {
            options.output_json_file = string(argv[i+1]);
            i++;
        } else if (arg == "--sa-temp") {
            options.simulated_annealing_temperature = stof(string(argv[i+1]));
            i++;
        }  else if (arg == "--sa-cooling") {
            options.simulated_annealing_cooling_rate = stof(string(argv[i+1]));
            i++;
        }

    }

    for (uint i = 0; i < focals.size(); i++)
        for (uint j = i + 1; j < focals.size(); j++)
            focal_pairs.push_back(make_pair(focals[i],focals[j]));

    Solver* solver;

    if (options.solver == kSoplex)
        solver = new SolverSoplex(focal_pairs,Solver::kOneMatrixAllPairs);
    else if (options.solver == kArmadillo)
        solver = new SolverArmadillo(focal_pairs,Solver::kOneMatrixAllPairs);
    else if (options.solver == kPetSc)
        solver = new SolverPetsc(focal_pairs,&argc,&argv,Solver::kOneMatrixPerPair);


    cout<<"Loading network file..."<<endl;
    solver->parseTextListFile(network_file);
    cout<<"Done"<<endl;

    PricingManager pm(options.budget);
    SimulatedAnnealing accepter;
    accepter.setTemperature(options.simulated_annealing_temperature);
    accepter.setCoolingRate(options.simulated_annealing_cooling_rate);
    cout<<"Compiling matrix for computation..."<<endl;
    LocalSearchEngine ls(focal_pairs,solver,&pm,&accepter,options);
    cout<<"Done"<<endl;

    JsonObject solutions;

    cout<<"Computing base solution (no investments)..."<<endl;
    ls.findBaseSolution();
    cout<<"Done"<<endl;
    Solution base_sol = ls.getBaseSolution();
    JsonObject* base_sol_json = base_sol.toJson();
    solutions.add("base_solution",base_sol_json);

    cout<<"Computing initial solution (greedy investments)..."<<endl;
    ls.findInitialSolution();
    cout<<"Done"<<endl;
    Solution initial_sol = ls.getInitialSolution();
    JsonObject* initial_sol_json = initial_sol.toJson();
    solutions.add("initial_solution",initial_sol_json);

    cout<<"Applying Local Search..."<<endl;
    Solution best_sol = ls.solve(&solutions);
    cout<<"Done"<<endl;
    JsonObject* best_sol_json = best_sol.toJson();
    solutions.add("best_solution",best_sol_json);


    if (options.output_json_file == "") {
        cout<<solutions.toString()<<endl;
    } else {
        std::ofstream file(options.output_json_file);
        try {
            file << solutions.toString()<< endl;
            cout<<"Saved solutions to "<<options.output_json_file<<endl;
        } catch (exception e) {
            cerr<<"Could not save JSON file in "<<options.output_json_file<<"."<<endl;
            cerr<<"Error is: "<<e.what()<<endl;
            cerr<<"Outputing to terminal instead:"<<endl;
            cout<<solutions.toString()<<endl;
        }
        file.close();
    }
    


    delete solver;

    return 0;

    
}
