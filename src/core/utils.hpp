#ifndef UTILS_HPP
#define UTILS_HPP
#include <sys/stat.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <exception>

#define MAX(a,b) ((a > b)? a :b)
#define MIN(a,b) ((a < b)? a :b)

inline bool file_exists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

class unimplemented_solver: public std::runtime_error {
    std::string msg;
public:
    unimplemented_solver(const std::string meth) :
        std::runtime_error("Unimplemented method:") {
        msg = "Unimplemented method for the chosen solver: " + meth;
    }
    virtual ~unimplemented_solver() throw() {}
    virtual const char *what() const throw() {
        return msg.c_str();
    }
};

struct id_val { //shorthand for a pair, with nicer names
    int id;
    double val;
    id_val() : id(0),val(0) {}
    static bool sort_by_val (id_val i, id_val j) { return (i.val < j.val); }
};

#endif
