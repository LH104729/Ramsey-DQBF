#include "abc_wrapper/abc_wrapper.hpp"
#include "common/dqcir_simplify.hpp"
#include "common/verilog.hpp"

#include <fstream>

Circuit gate_to_circuit(Gate& g) {
    Circuit c(g.name);
    c.operation = g.operation;
    for (auto& i : g.inputs) {
        if (i.sign) {
            c.children.push_back(Circuit(i.name));
        } else {
            c.children.push_back(~Circuit(i.name));
        }
    }
    return c;
}

Gate circuit_to_gate(Circuit& c) {
    Gate g;
    g.name = c.name;
    g.operation = c.operation;
    for (auto& ch : c.children) {
        if (ch.operation == "") {
            g.inputs.push_back({ch.name, true});
        } else if (ch.operation == "~" && ch.children[0].operation == "") {
            g.inputs.push_back({ch.children[0].name, false});
        } else {
            printf("Error: %s\n", ch.to_string().c_str());
        }
    }
    return g;
}


void dqcir_simplify(dqcir& p) {
    VerilogModule v("ramsey");
    for (auto& u : p.u_vars) {
        v.PI.push_back(u);
    }
    for (auto& e : p.e_vars) {
        v.PI.push_back(e.first);
    }
    v.PO.push_back(p.out_var);
    for (auto& g : p.gates) {
        v.wires.push_back(Wire(g.name, gate_to_circuit(g)));
    }

    std::ofstream file("ramsey.v");
    file << v.to_string();
    file.close();

    ABC_wrapper abc;
    abc.v_to_v_fraig("ramsey.v", "ramsey_opt.v");

    VerilogModule v_opt = VerilogModule::from_file("ramsey_opt.v", false);

    p.gates.clear();
    for (auto& w : v_opt.wires) {
        w.assign.name = w.name;
        p.gates.push_back(circuit_to_gate(w.assign));
    }
}