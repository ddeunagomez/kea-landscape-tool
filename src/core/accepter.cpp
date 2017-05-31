#include "accepter.hpp"
#include <stdlib.h>
#include <math.h>

bool Accepter::accept(double v) {
    accepted = v;
    return true;
}

bool AcceptBetter::accept(double v) {
    if (v < getAccepted())
        return Accepter::accept(v);
    return false;
}

bool SimulatedAnnealing::accept(double v) {
    double val = (double)rand()/(double)(RAND_MAX);

    double prob = exp((getAccepted() - v)/temp);

    temp *= cr;
    if (temp < 1.0) temp = 1.0;

    if (val < prob)
        return Accepter::accept(v);
    return false;
}
