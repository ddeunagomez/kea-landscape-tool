#ifndef ACCEPTER_H
#define ACCEPTER_H

class Accepter {

    double accepted_objective_;
public:
    Accepter() {} 

    inline void reset(double initial) {
        accepted_objective_ = initial;
    }
    inline double getAccepted() const {
        return accepted_objective_;
    }
    bool accept(double value);
};

class AcceptBetter : public Accepter {

public:
    AcceptBetter() : Accepter() {}
    bool accept(double value);
};

class SimulatedAnnealing : public Accepter {
    double temperature_;
    double cooling_rate_;
public:
    SimulatedAnnealing() : Accepter(),temperature_(1),cooling_rate_(1) {}
    inline void setTemperature(double t) {
        temperature_ = t;
    }
    inline void setCoolingRate(double _cr) {
        cooling_rate_ = _cr;
    }
    bool accept(double value);
};

#endif
