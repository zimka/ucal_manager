#include "core.h"
#include <device/acquire.h>
#include <common/exceptions.h>

using namespace runtime;

void loadBlock(std::unique_ptr<device::IDevice> const& device, Block block) {
    device->setReadingSampling(block.sampling_step_tu);

    if (block.isFinite()) {
        device->setDuration(block.block_len_tu);
    }
    if (!(block.isReadOnly())) {
        device->setProfiles({
            {common::ControlKey::Vg, block.guard},
            {common::ControlKey::Vm, block.mod},
            }, block.pattern_len_tu);
    }
    device->prepare();
}

common::MachineState CoreState::getState() {
    // This method should not be delegated to CoreState
    throw common::AssertionError("getState method call was delegated to CoreState");
}

common::Config const& CoreState::getConfig() {
    update();
    return *common::acquireConfig();
}

Plan const& CoreState::getPlan() {
    update();
    // TODO: have to change signature 
    //if (current_block_ind_ > 0) {
    //    return Plan(plan_.begin() + current_block_ind_, plan.end());
    //}
    return plan_;
}

storage::Storage const& CoreState::getData() {
    update();
    return storage_;
}

void CoreState::setPlan(Plan plan) {
    update();
    if (plan.size()) {
        validatePlan(plan);
    }
    if (worker_thread_.joinable()) {
        // TODO: maybe better join?
        throw common::AssertionError("Thread must be inactive during setPlan");
    }

    plan_ = plan;
    storage_.reset();
    // Drop old data if it is somehow still there
    data_queue_ = FrameQueue();
}

void CoreState::setConfig(json const& json_data){
    auto config = common::acquireConfig();
    auto it = json_data.begin();
    while (it != json_data.end()) {
        std::string error_msg = "Invalid config: " + json_data.dump();
        error_msg += "; failed on key " + it.key();
        bool set_value = false;
        try {
            if (common::ConfigDoubleKey::_is_valid(it.key().c_str())) {
                auto typed_key = common::ConfigDoubleKey::_from_string(it.key().c_str());
                double value = it.value().get<double>();
                bool status = config->write(typed_key, value);
                if (!(status)) {
                    throw common::ValueError(error_msg);
                }
                set_value = true;
            }

            if (common::ConfigStringKey::_is_valid(it.key().c_str())) {
                auto typed_key = common::ConfigStringKey::_from_string(it.key().c_str());
                std::string value = it.value().get<std::string>();
                bool status = config->write(typed_key, value);
                if (!(status)) {
                    throw common::ValueError(error_msg);
                }
                set_value = true;
            }
        } catch (json::type_error) {
            //will throw in next line if failed to set value for any reason
        }
        if (!(set_value)) {
            throw common::ValueError(error_msg);
        }
        ++it;
    }
};

void CoreState::stop() {
    current_block_ind_ = -1;
    if (worker_thread_.joinable()) {
        // Waiting for worker thread to finish
        worker_thread_.join();
    }
    update();
    storage_.finalize();
}

void CoreState::runNext() {
    // if already running
    if (current_block_ind_ >= 0) {
        if (!(worker_thread_.joinable())) {
            // TODO: restart thread?
            throw common::AssertionError("Worker thread have died");
        }
        if ((plan_.size() - current_block_ind_) > 1) {
            // There is at least one block in plan
            current_block_ind_++;
        }
        else {
            current_block_ind_ = -1;
            // TODO: join thread?
        }
        return;
    }
    current_block_ind_ = 0;
    worker_thread_ = std::thread(
        [](std::atomic<int8_t>* master_block_ind, CoreState::FrameQueue* queue, Plan plan) {
            auto worker = Worker(master_block_ind, queue, plan);
            while (!worker.finished()) {
                // TODO: some sleep?
                worker.doStep();
            }
        },
        &current_block_ind_, &data_queue_, plan_
    );
}

void CoreState::validatePlan(Plan plan) {
    auto device = device::acquireDevice();
    for (auto block : plan) {
        loadBlock(device, block);
    }
}

void CoreState::update() {
    /*
    while (data_queue_.peek() != nullptr) {
        storage::Frame frame;
        bool status = data_queue_.try_dequeue(frame);
        if (status){
            storage_.append(std::move(frame));
        }
    }
    */
}

Worker::Worker(std::atomic<int8_t>* master_block_ind, CoreState::FrameQueue * queue, Plan plan): global_block_ind_(master_block_ind), queue_(queue), plan_(plan) {
    device_ = device::acquireDevice();
    worker_block_ind_ = -1;
}

void Worker::doStep() {
    if (finished()) {
        return;
    }
    //fix value at given step
    int8_t master_block_ind = *global_block_ind_;
    bool is_sync = (master_block_ind) == worker_block_ind_;
    bool is_running = (device_->getState() == +device::DeviceState::Running);
    if ((is_sync) && (is_running)) {
        // Sync: worker is synced with master and device running
        // then: transfer data to master, remains Sync or becomes Timeout
        auto frame = device_->getData();
        if (frame.size()) {
            queue_->enqueue(std::move(frame));
        }
    }
    else if ((!is_sync) && (!is_running)) {
        // Ready: worker is not running block and master says to run new block
        // then: run new block, becomes Sync
        if (master_block_ind < 0) {
            return;
        }
        loadBlock(device_, plan_[master_block_ind]);
        device_->run();
        worker_block_ind_ = master_block_ind;
    }
    else if ((!is_sync) && (is_running)) {
        //Lag: worker is running some block, but master wants to run another block
        //then: stop running current block, becomes Ready
        device_->stop();
    }
    else if ((is_sync) && (!is_running)) {
        //Timeout: worker is synced with master, but device has finished block on its own
        //then: move forward master index, becomes Ready
        if ((worker_block_ind_ + 1) < plan_.size()) {
            // run next block if can
            *global_block_ind_ = worker_block_ind_ + 1;
        }
        else {
            // finish execution
            *global_block_ind_ = -1;
        }
    }
};

bool Worker::finished() {
    if ((*global_block_ind_) == -1) {
        return true;
    }
    return false;
}