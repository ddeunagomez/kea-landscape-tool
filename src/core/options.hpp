#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <string>
#include <vector>
#include "electrical_circuit.hpp"
#include "solver_wrapper.hpp"

struct KeaOptions {

    AvailableSolvers solver;
    std::string output_json_file;
    float time_limit;
    uint max_iterations;
    uint destruction_rate;
    int budget;
    double simulated_annealing_temperature;
    double simulated_annealing_cooling_rate;

    KeaOptions() {
        //Default initialization:

        solver = kPetSc;
        output_json_file = "";
        time_limit = 200.0;
        max_iterations = 50;
        destruction_rate = 20;
        budget = 0;
        simulated_annealing_temperature = 5.0;
        simulated_annealing_cooling_rate = 0.98;

    }

};


#endif // OPTIONS_HPP
