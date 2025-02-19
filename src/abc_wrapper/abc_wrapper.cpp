#include "abc_wrapper/abc_wrapper.hpp"
#include "common/utils.hpp"

#include <boost/process.hpp>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>

ABC_wrapper::ABC_wrapper(bool verbose) {
    this->verbose = verbose;
    pAbc = Abc_FrameGetGlobalFrame();
}

ABC_wrapper::~ABC_wrapper() {
    Abc_Stop();
}

int ABC_wrapper::exec(std::string command) {
    return Cmd_CommandExecute(pAbc, command.c_str());
}
int ABC_wrapper::read(std::string filename, bool fraig) {
    int status = exec(("read " + filename).c_str());
    if (status != 0) {
        return status;
    }
    if (fraig) {
        return exec("fraig");
    } else {
        return exec("strash");
    }
}

void ABC_wrapper::v_to_v_fraig(std::string input_file, std::string output_file, bool keep_input) {
    print_info("ABC: Converting Verilog to AIG with fraig");
    int status;
    status = read(input_file, true);
    if (status != 0) {
        print_info(("ABC exited with status " + std::to_string(status)).c_str());
        return;
    }
    status = exec("dc2");
    if (status != 0) {
        print_error(("ABC exited with status " + std::to_string(status)).c_str());
        return;
    }
    status = exec("write_verilog " + output_file);
    if (!keep_input) {
        std::filesystem::remove(input_file);
    }
    if (status == 0) {
        return;
    }
    print_error(("ABC exited with status " + std::to_string(status)).c_str());
}