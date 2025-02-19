#include "common/dqdimacs.hpp"
#include "common/dqcir_simplify.hpp"
#include "common/dqcir.hpp"
#include "common/utils.hpp"

#include <cxxopts.hpp>
#include <iostream>
#include <string>
#include <set>
#include <vector>

int main(int argc, char** argv) {
    cxxopts::Options options("ramsey", "Generate DQBF instance of Ramsey number n < R(s, t) for n >= t >= s > 2");

    options.add_options()
        ("s", "s", cxxopts::value<int>())
        ("t", "t", cxxopts::value<int>())
        ("n", "n", cxxopts::value<int>())
        ("fraig", "Optimize circuit with fraig", cxxopts::value<bool>()->default_value("false"))
        ("tseitin", "Output in CNF format", cxxopts::value<bool>()->default_value("false"))
        ("output", "Output directory", cxxopts::value<std::string>()->default_value("./"))
        ("h,help", "Print usage");


    auto result = options.parse(argc, argv);

    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        exit(0);
    }

    if (!result.count("s") || !result.count("t") || !result.count("n")) {
        print_error("Please specify s, t and n");
    }

    int s = result["s"].as<int>();
    int t = result["t"].as<int>();
    int n = result["n"].as<int>() - 1;
    if (t < s) {
        std::swap(t, s);
    }
    if (n + 1 < t) {
        print_error("n must be greater or equal to t");
    }

    dqcir p;

    int n_bits = log_2(n) + 1;
    std::vector<std::vector<std::string>> x(t, std::vector<std::string>(n_bits));
    for (int i = 0; i < t; i++) {
        for (int j = 0; j < n_bits; j++) {
            x[i][j] = "x_" + std::to_string(i) + "_" + std::to_string(j);
            p.u_vars.push_back(x[i][j]);
        }
    }

    std::vector<std::vector<int>> m(t, std::vector<int>(t, -1));
    for (int i = 0; i < t; i++) {
        for (int j = i + 1; j < t; j++) {
            m[i][j] = p.e_vars.size();
            p.e_vars.push_back({"y_" + std::to_string(m[i][j]), {}});
            for (int k = 0; k < n_bits; k++) {
                p.e_vars.back().second.push_back(x[i][k]);
            }
            for (int k = 0; k < n_bits; k++) {
                p.e_vars.back().second.push_back(x[j][k]);
            }
        }
    }

    p.out_var = "R";

    for (int i = 0; i < t; i++) {
        for (int j = i + 1; j < t; j++) {
            std::vector<Input> child;
            for (int k = 0; k < n_bits; k++) {
                std::string name = "eq_x_" + std::to_string(i) + "_" + std::to_string(j) + "_" + std::to_string(k);
                p.gates.push_back({name + "_1", "|", {{x[i][k], false}, {x[j][k], true}}});
                p.gates.push_back({name + "_2", "|", {{x[i][k], true}, {x[j][k], false}}});
                p.gates.push_back({name, "&", {{name + "_1", true}, {name + "_2", true}}});
                child.push_back({name, true});
            }
            p.gates.push_back({"eq_x_" + std::to_string(i) + "_" + std::to_string(j), "&", child});
        }
    }

    // Force each y to have the same value on the same input
    std::set<std::pair<int, int>> pairs;
    for (int i = 0; i < t; i++) {
        for (int j = i + 2; j < t; j++) {
            pairs.insert({m[i][i+1], m[i][j]});
        }
    }
    for (int i = 1; i < t - 1; i++) {
        pairs.insert({m[0][t-1], m[i][t-1]});
    }
    for (int i = 1; i < t * (t - 1) / 2; i++) {
        pairs.insert({i-1, i});
    }
    if (t > 3) {
        pairs.insert({0, t * (t - 1) / 2 - 1});
    } else {
        pairs.insert({0, 2});
    }

    for (auto& [i, j] : pairs) {
        std::string name = "eq_y_" + std::to_string(i) + "_" + std::to_string(j);
        p.gates.push_back({name + "_1", "|", {{p.e_vars[i].first, false}, {p.e_vars[j].first, true}}});
        p.gates.push_back({name + "_2", "|", {{p.e_vars[i].first, true}, {p.e_vars[j].first, false}}});
        p.gates.push_back({name, "&", {{name + "_1", true}, {name + "_2", true}}});
    }

    std::vector<Input> rules;
    // Colouring agrees
    for (int i = 0; i < t; i++) {
        for (int j = i + 2; j < t; j++) {
            // x_{i+1} = x_j -> y_{m[i][i+1]} = y_{m[i][j]}
            p.gates.push_back({"R_" + std::to_string(i) + "_" + std::to_string(j), "|", {{"eq_x_" + std::to_string(i + 1) + "_" + std::to_string(j), false}, {"eq_y_" + std::to_string(m[i][i+1]) + "_" + std::to_string(m[i][j]), true}}});
            rules.push_back({"R_" + std::to_string(i) + "_" + std::to_string(j), true});
        }
    }

    for (int i = 1; i < t - 1; i++) {
        // x_0 = x_i -> y_{m[0][t-1]} = y_{m[i][t-1]}
        p.gates.push_back({"R_" + std::to_string(i), "|", {{"eq_x_0_" + std::to_string(i), false}, {"eq_y_" + std::to_string(m[0][t-1]) + "_" + std::to_string(m[i][t-1]), true}}});
        rules.push_back({"R_" + std::to_string(i), true});
    }

    // Colourings are symmetric

    if (t > 3) {
        p.gates.push_back({"R_0", "|", {{"eq_x_0_" + std::to_string(t - 1), false}, {"eq_x_1_" + std::to_string(t - 2)}, {"eq_y_0_" + std::to_string(t * (t - 1) / 2 - 1), true}}});
        rules.push_back({"R_0", true});
    }
    if (t == 3) {
        p.gates.push_back({"R_0", "|", {{"eq_x_0_2", false}, {"eq_y_0_2", true}}});
        rules.push_back({"R_0", true});
    }

    // x_i != x_j pairwise -> one of y_i != y_j
    {
        std::vector<Input> child;
        for (int i = 0; i < s; i++) {
            for (int j = i + 1; j < s; j++) {
                child.push_back({"y_" + std::to_string(m[i][j]), true});
            }
        }
        p.gates.push_back({"all_y", "&", child});
    }

    {
        std::vector<Input> child;
        for (auto& v : p.e_vars) {
            child.push_back({v.first, false});
        }
        p.gates.push_back({"none_y", "&", child});
    }

    p.gates.push_back({"con_y", "&", {{"all_y", false}, {"none_y", false}}});

    {
        std::vector<Input> child;
        for (int i = 0; i < t; i++) {
            for (int j = i + 1; j < t; j++) {
                child.push_back({"eq_x_" + std::to_string(i) + "_" + std::to_string(j), true});
            }
        }
        child.push_back({"con_y", true});
        p.gates.push_back({"R_" + std::to_string(t), "|", child});
        rules.push_back({"R_" + std::to_string(t), true});
    }

    // Conditioning on x_i < n
    if ((1 << n_bits) != n + 1) {
        std::vector<bool> bit_n(n_bits, false);
        for (int i = 0; i < n_bits; i++) {
            if ((n >> i) & 1) {
                bit_n[n_bits - i - 1] = true;
            }
        }

        int cnt = 0;
        std::vector<Input> child;
        for (int i = 0; i < t; i++) {
            std::vector<Input> ones;
            for (int j = 0; j < n_bits; j++) {
                if (bit_n[j]) {
                    ones.push_back({x[i][j], false});
                } else {
                    p.gates.push_back({"C_" + std::to_string(cnt), "|", ones});
                    p.gates.back().inputs.push_back({x[i][j], false});
                    child.push_back({"C_" + std::to_string(cnt++), false});
                }
            }
        }
        p.gates.push_back({"R_", "&", rules});
        p.gates.push_back({"R", "|", child});
        p.gates.back().inputs.push_back({"R_", true});
    } else {
        p.gates.push_back({"R", "&", rules});
    }

    std::string file_name = "R_" + std::to_string(s) + "_" + std::to_string(t) + "_" + std::to_string(n + 1);

    if (result["fraig"].as<bool>()) {
        dqcir_simplify(p);
    }

    if (result["tseitin"].as<bool>()) {
        dqdimacs q;
        q.from_file(p);
        q.to_file(result["output"].as<std::string>() + file_name + ".dqdimacs");
    } else {
        p.to_file(result["output"].as<std::string>() + file_name + ".dqcir");
    }

}