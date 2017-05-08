#include <sys/stat.h>
#include <string>
#include <iostream>

#define MAX(a,b) ((a > b)? a :b)
#define MIN(a,b) ((a < b)? a :b)

#define UNSUPPORTED  do { \
        std::cerr<<"Unsupported function "<<__FILE__<<":"<<__LINE__<<std::endl; \
    } while(0)                                                          

inline bool file_exists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

