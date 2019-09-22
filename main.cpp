#include <iostream>
#include <common/measures.h>
#include <runtime/state_machine.h>
#include <sstream>
#include <json/single_include/nlohmann/json.hpp>


int main(){
    std::cout << common::ACCURACY_ERROR << std::endl;
    using runtime::Context;
    using runtime::Plan;
    Context defaultContext = {
        Plan(),
        std::make_unique<device::MockDevice>(),
        storage::Storage()
    };
    runtime::StateMachine machine (std::move(defaultContext)); // TODO: check if this works
    // TODO: read config from config file or from default
    //machine.setConfig(); // Config will be read automatically from "override_config.json"
    // TODO: read plan from file
    const char* plan_input = "single_plan.json";
    std::ifstream file(plan_input);
    std::stringstream content_s;
    content_s << file.rdbuf();
    if (content_s.str().empty()) {
        throw std::runtime_error ("File with plans is empty");
    }
    auto plan_js = json::parse(content_s.str());
    Plan plan = plan_js.get<Plan>();
    machine.setPlan(plan);
    // TODO: execute plan and write data to output file
    machine.runNext();
    auto const& storage = machine.getData();
    json output;
    for (auto& frame : storage) {
        output.push_back(frame);
    }
    std::ofstream ofile("data.json");
    ofile << output;
    // Shutdown
}
