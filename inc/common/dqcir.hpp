#ifndef DQCIR_HPP
#define DQCIR_HPP

#include "common/circuit.hpp"

#include <string>
#include <unordered_map>
#include <vector>

// Data structure for DQBF in circuit form
class dqcir {
   public:
    // Number of variables
    uint64_t var_cnt;

    dqcir();

    // Print problem info
    void print_stat(bool detailed = false);

    // Universal variables, (name)
    std::vector<std::string> u_vars;

    // Existential variables, (name, dependency set)
    std::vector<std::pair<std::string, std::vector<std::string>>> e_vars;

    std::string out_var;
    std::vector<Gate> gates;

    // Print to file
    void to_file(std::string path);
};
#endif