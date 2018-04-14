#include "accepter.hpp"
#include <stdlib.h>
#include <math.h>

bool Accepter::accept(double v) {
    accepted_objective_ = v;
    return true;
}

bool AcceptBetter::accept(double v) {
    if (v < getAccepted())
        return Accepter::accept(v);
    return false;
}

bool SimulatedAnnealing::accept(double v) {
    double val = (double)rand()/(double)(RAND_MAX);

    double prob = exp((getAccepted() - v)/temperature_);

    temperature_ *= cooling_rate_;
    if (temperature_ < 1.0) temperature_ = 1.0;

    if (val < prob)
        return Accepter::accept(v);
    return false;
}
