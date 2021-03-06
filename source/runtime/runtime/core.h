#ifndef UCAL_MANAGER_RUNTIME_CORE_H
#define UCAL_MANAGER_RUNTIME_CORE_H
#include "i_state.h"
#include <device/device.h>
#include <readerwriterqueue/readerwriterqueue.h>
#include <thread>
#include <atomic>

namespace runtime {
    /*
    Actions implementation for state machine without state validation.
    Must be used inside state machine only, otherwise is heavily unsafe.
    Uses separate thread to load data from device.
    */
    using FrameQueue = moodycamel::ReaderWriterQueue<storage::Frame>;

    class CoreState : public IState {
    public:
        ~CoreState() override;
        common::MachineState getState() override;

        json getConfig() override; 

        Plan getPlan() override;

        storage::Storage const& getData() override;

        void setConfig(json const& json_data) override;

        void setPlan(Plan plan) override; 

        void runNext() override;

        void stop() override;

        bool isRunning();

    private:
        std::atomic<int8_t> current_block_ind_ {-1};
        FrameQueue data_queue_;
        storage::Storage storage_;
        Plan plan_;
        std::thread worker_thread_;

        void update();
    };

    class Worker {
    public:
        Worker(std::atomic<int8_t>* master_block_ind, FrameQueue* queue, Plan plan);
        void doStep(); 
        void finalize();
        bool finished();
    private:
        Plan plan_;
        FrameQueue* queue_;
        std::atomic<int8_t>* global_block_ind_;
        int8_t worker_block_ind_;
        std::unique_ptr<device::IDevice> device_;
    };
}
#endif
