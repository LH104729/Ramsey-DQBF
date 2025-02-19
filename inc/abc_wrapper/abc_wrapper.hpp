#ifndef ABC_WRAPPER_HPP
#define ABC_WRAPPER_HPP

#include <string>
#include <unordered_map>

extern "C"
{
    #define ABC_USE_STDINT_H
    #include "misc/util/abc_global.h"
    #include "misc/util/abc_namespaces.h"
    #include "misc/vec/vec.h"
    #include "base/abc/abc.h"
    #include "base/main/abcapis.h"
    #include "base/main/main.h"
    #include "base/main/mainInt.h"
    #include "bdd/bbr/bbr.h"
    #include "bdd/cudd/cudd.h"

}


enum ABC_result {
    SAT,
    UNSAT,
    TIMEOUT,
    UNKNOWN
};

class ABC_wrapper {
  private:
    bool verbose = false;
    Abc_Frame_t * pAbc;

  public:
    ABC_wrapper(bool verbose = false);
    ~ABC_wrapper();

    int exec(std::string command);
    int read(std::string filename, bool fraig = false);
    
    void v_to_v_fraig(std::string input_file, std::string output_file, bool keep_input = false);
};

#endif
