#include <iostream>
#include <common/measures.h>
#include <common/exceptions.h>
#include <runtime/state_machine.h>
#include <runtime/core.h>

#include <sstream>
#include <json/single_include/nlohmann/json.hpp>
#include <storage/storage.h>

#include <device/acquire.h>
#include <runtime/core.h>


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
    runtime::StateMachine machine;
    machine.setState(runtime::createState<common::MachineState::NoPlan>(&machine));
    auto pln = readPlan(plan_input);
    machine.setPlan(pln);
    machine.runNext();
    
    int n = 0;
    bool not_finished = true;
    while ((n < 60) && not_finished) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        auto const& stor = machine.getData();
        std::cout << "Collected: " << stor.size() << std::endl;
        if (n == 10) {
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

void loadBlock2(std::unique_ptr<device::IDevice> const& device, runtime::Block block) {
    device->setReadingSampling(block.sampling_step_tu);

    device->setDuration(block.block_len_tu);
    if ((block.guard.size() || block.mod.size())) {
        device->setProfiles({
            {common::ControlKey::Vg, block.guard},
            {common::ControlKey::Vm, block.mod},
            }, block.pattern_len_tu);
    }
    device->prepare();
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
