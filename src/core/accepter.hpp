#ifndef ACCEPTER_H
#define ACCEPTER_H

class Accepter {

    double accepted;
public:
    Accepter() {} 

    inline void reset(double initial) {
        accepted = initial;
    }
    inline double getAccepted() const {
        return accepted;
    }
    bool accept(double value);
};

class AcceptBetter : public Accepter {

public:
    AcceptBetter() : Accepter() {}
    bool accept(double value);
};

class SimulatedAnnealing : public Accepter {
    double temp;
    double cr;
public:
    SimulatedAnnealing() : Accepter(),temp(1),cr(1) {}
    inline void setTemperature(double t) {
        temp = t;
    }
    inline void setCoolingRate(double _cr) {
        cr = _cr;
    }
    bool accept(double value);
};

#endif
