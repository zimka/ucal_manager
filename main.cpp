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
    while (n < 40) {
        n++;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        auto const& stor = machine.getData();
        std::cout << stor.size() << std::endl;
        if (n == 10) {
            machine.runNext();
            std::cout << "FORWARD!" << std::endl;
        }
    }
    std::cout << "Finished!" << std::endl;
    dumpData(machine.getData(), common::TimeStamp(0, 0));
    machine.stop();
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

int device_run() {
    std::string plan_input = "single_block.json";
    storage::Storage store;
    auto pln = readPlan(plan_input);
    auto dev = device::acquireDevice();
    for (auto block : pln) {
        loadBlock2(dev, block);
        dev->run();
        std::cout << "block!" << std::endl;
        for (auto i = 0; i < 100; i++) {
            if (dev->getState() != +device::DeviceState::Running) {
                break;
            }
            store.append(dev->getData());
            std::cout<<store.size() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        if (dev->getState() == +device::DeviceState::Running) {
            dev->stop();
        }
    }
    dumpData(store, common::TimeStamp(0, 0));
    return 1;
}

int worker_run() {
    std::string plan_input = "single_block.json";

    auto queue = runtime::FrameQueue();
    std::atomic<int8_t> master_block_ind = 0;
    auto pln = readPlan(plan_input);
    int8_t prev_value = 0;
    int32_t prev_value_count = 0;
    try {
        auto worker = runtime::Worker(&master_block_ind, &queue, pln);
        while (!worker.finished()) {
            // TODO: some sleep?
            worker.doStep();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            auto current = master_block_ind.load();
            if (current == prev_value) {
                prev_value_count++;
            }
            else {
                std::cout << "New ind! " << std::to_string(current) << ", steps:"<< prev_value_count<< std::endl;
                prev_value = current;
                prev_value_count = 0;
            }
            if ((prev_value_count) > 100) {
                master_block_ind.fetch_add(1);
            }
        }
        std::cout << "Worker finished" << std::endl;
    } catch (common::UcalManagerException& exc) {
        std::cout << "WORKER EXCEPTION!! " << exc.what() << std::endl;
    }
    auto store = storage::Storage();
    while (queue.peek() != nullptr) {
        storage::Frame frame;
        bool status = queue.try_dequeue(frame);
        if (status) {
            store.append(std::move(frame));
        }
    }
    dumpData(store, common::TimeStamp(0, 0));
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