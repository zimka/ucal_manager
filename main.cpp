#include <iostream>
#include <common/measures.h>
#include <common/exceptions.h>
#include <runtime/state_machine.h>
#include <runtime/core.h>

#include <sstream>
#include <json/single_include/nlohmann/json.hpp>
#include <storage/storage.h>

#include <device/device.h>
#include <service/server_impl.h>


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

json readConfig(std::string filename) {
    std::ifstream file(filename);
    std::stringstream content_s;
    content_s << file.rdbuf();
    if (content_s.str().empty()) {
        return {};
    }
    return json::parse(content_s.str());
}


common::TimeStamp dumpData(storage::Storage const& str, common::TimeStamp start_ts) {
    json output;
    common::TimeStamp last_ts(0, 0);
    for (auto it=str.afterTs(start_ts); it!=str.end(); it++) {
        output.push_back(it->repr());
        last_ts = it->getTs();
    }
    std::ofstream ofile("data.json");
    ofile << output;
    return last_ts;
}

int main_run(){
    std::string plan_input = "single_block.json";
    std::string config_input = "setup_config.json";

    runtime::StateMachine machine;
    device::DeviceTimer devt(100) ;
    std::cout << "Starts from NoPlan: " << (machine.getState() == +common::MachineState::NoPlan) << std::endl;
    std::cout << "Start config: " << machine.getConfig().dump() << std::endl;
    machine.setConfig(readConfig(config_input));
    std::cout << "Final config: " << machine.getConfig().dump() << std::endl;
    auto pln = readPlan(plan_input);
    machine.setPlan(pln);
    machine.runNext();
    
    int n = 0;
    bool not_finished = true;
    devt.run();
    while ((n < 60) && not_finished) {
        auto stamp = devt.getStamp();
        uint32_t ms = (n+1) * 1000 - stamp.value();
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        auto const& stor = machine.getData();
        std::cout << n << ") Collected: " << stor.size() << "; ";
        std::cout << stamp.repr() << "|"<< ms << std::endl;
        if (n == 9) {
            machine.runNext();
            std::cout << "Run Next!" << std::endl;
        }
        dumpData(machine.getData(), common::TimeStamp(0, 0));
        not_finished = (machine.getState() == +common::MachineState::Executing);
        n++;
    }
    dumpData(machine.getData(), common::TimeStamp(0, 0));
    if (not_finished){
        std::cout << "Manual stop!" << std::endl;
        machine.stop();
    }
    std::cout << "Finished!" << std::endl;
    return 0;
}

int main_service() {
    service::RunService();
    return 0;
}

int main() {
    try {
        //main_run();
        return main_service();
    }
    catch (common::UcalManagerException& e) {
        std::cout << "Failed with exception:" << e.what()<< std::endl;
    }
    system("pause");
    return 0;
}
