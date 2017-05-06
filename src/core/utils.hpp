#include <sys/stat.h>
#include <string>

#define MAX(a,b) ((a > b)? a :b)
#define MIN(a,b) ((a < b)? a :b)

inline bool file_exists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

