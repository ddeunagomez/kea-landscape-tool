#ifndef UTILS_HPP
#define UTILS_HPP
#include <sys/stat.h>
#include <string>
#include <iostream>

#define MAX(a,b) ((a > b)? a :b)
#define MIN(a,b) ((a < b)? a :b)

#define UNSUPPORTED  do { \
        std::cerr<<"Unsupported function "<<__FILE__<<":"<<__LINE__<<std::endl; \
        std::cerr<<"Terminating now"<<std::endl;                        \
        exit(1);                                \
    } while(0)                                                          

inline bool file_exists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

#endif
