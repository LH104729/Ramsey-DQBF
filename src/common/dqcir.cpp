#include "common/dqcir.hpp"
#include "common/utils.hpp"
#include <fstream>
#include <string>
#include <unordered_map>

dqcir::dqcir() {
    
}

void dqcir::print_stat(bool detailed) {
    printf("-------- Stat --------\n%lu universal var(s):\n", u_vars.size());
    if (detailed) {
        for (auto& x : u_vars) {
            printf("%s ", x.c_str());
        }
        putchar_unlocked('\n');
    }
    printf("%zu existential var(s):\n", e_vars.size());
    if (detailed) {
        for (auto& y : e_vars) {
            printf("%s: ", y.first.c_str());
            for (auto& x : y.second) {
                printf("%s ", x.c_str());
            }
            putchar_unlocked('\n');
        }
    }
}

void dqcir::to_file(std::string path) {
    std::ofstream file(path);
    if (!file) {
        throw std::runtime_error("Error on opening file");
    }
    file << "#QCIR-14\n";
    file << "forall(" << join(u_vars, ", ") << ")\n";
    for (auto& [y, v] : e_vars) {
        file << "depend(" << y << ", " << join(v, ", ") << ")\n";
    }
    file << "output(" << out_var << ")\n\n";
    
    std::unordered_map<std::string, std::string> gate_map = {
        {"&", "and"},
        {"|", "or"},
    };

    for (auto& gate : gates) {
        std::vector<std::string> children;
        for (auto& [name, sign] : gate.children) {
            children.push_back((sign ? "" : "-") + name);
        }
        file << gate.name << " = " << gate_map[gate.operation] << "(" << join(children, ", ") << ")\n";
    }
    file.close();
}