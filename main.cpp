#include <iostream>
#include <common/measures.h>
#include <common/exceptions.h>
#include <runtime/state_machine.h>
#include <runtime/core.h>

#include <sstream>
#include <json/single_include/nlohmann/json.hpp>
#include <storage/storage.h>


runtime::Plan readPlan(std::string filename) {
    std::ifstream file(filename);
    std::stringstream content_s;
    content_s << file.rdbuf();
    if (content_s.str().empty()) {
        throw common::ValueError("File with plans is empty");
    }
    std::cout << content_s.str() << std::endl;
    auto plan_js = json::parse(content_s.str());
    runtime::Plan plan = plan_js.get<runtime::Plan>();
    return plan;
}

common::TimeStamp dumpData(storage::Storage const& str, common::TimeStamp start_ts) {
    json output;
    common::TimeStamp last_ts(0, 0);
    for (auto& it=str.afterTs(start_ts); it!=str.end(); it++) {
        output.push_back(it->repr());
        last_ts = it->getTs();
    }
    std::ofstream ofile("data.json");
    ofile << output;
    return last_ts;
}

int main_run(){
    std::string plan_input = "single_block.json";
    runtime::StateMachine machine;
    machine.setState(runtime::createState<common::MachineState::NoPlan>(&machine));
    auto pln = readPlan(plan_input);
    machine.setPlan(pln);
    machine.runNext();
    int n = 0;
    while (n < 30) {
        n++;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        auto const& stor = machine.getData();
        std::cout << stor.size() << std::endl;
    }
    dumpData(machine.getData(), common::TimeStamp(0, 0));
    machine.stop();
    return 0;
}
int main_run3() {
    std::string plan_input = "single_block.json";

    auto cr = std::make_unique<runtime::CoreState>();
    auto pln = readPlan(plan_input);
    cr->setPlan(pln);
    system("pause");
    return 0;
}


int main() {
    try {
        main_run();
    }
    catch (common::UcalManagerException& e) {
        std::cout << "Failed with exception:" << e.what()<< std::endl;
    }
    system("pause");
}