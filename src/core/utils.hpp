#ifndef UTILS_HPP
#define UTILS_HPP
#include <sys/stat.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <exception>
#include <vector>

#define MAX(a,b) ((a > b)? a :b)
#define MIN(a,b) ((a < b)? a :b)

inline bool file_exists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

class unimplemented_solver: public std::runtime_error {
    std::string msg_;
public:
    unimplemented_solver(const std::string meth) :
        std::runtime_error("Unimplemented method:") {
        msg_ = "Unimplemented method for the chosen solver: " + meth;
    }
    virtual ~unimplemented_solver() throw() {}
    virtual const char *what() const throw() {
        return msg_.c_str();
    }
};

struct id_val { //shorthand for a pair, with nicer names
    int id;
    double val;
    id_val(int _id = 0, double _val = 0) : id(_id),val(_val) {}
    static bool sort_by_val (id_val i, id_val j) { return (i.val < j.val); }
    static bool sort_by_id  (id_val i, id_val j) { return (i.id < j.id); }
};


#endif
