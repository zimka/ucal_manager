#ifndef UCAL2_DEVICE_DEVICE_H
#define UCAL2_DEVICE_DEVICE_H

#include "common/measures.h"
#include "common/keys.h"
#include "better_enums/enum.h"
#include "storage/frame.h"
#include "timer.h"
#include <stdint.h>
#include <vector>
#include <map>
#include <string>


namespace device {
    using VoltageProfile = std::vector<common::VoltUnit>;
    using ProfileSetup = std::map<common::ControlKey, VoltageProfile>;
    BETTER_ENUM(DeviceState, char, CanSet, Prepared, Running);

    class IDevice {
    public:
        virtual void setProfiles(const ProfileSetup &profiles, common::TimeUnit profile_length) = 0;

        virtual void setDuration(common::TimeUnit total_duration) = 0;

        virtual void setReadingSampling(common::TimeUnit step, uint16_t per_points = 1) = 0;

        virtual std::string getSetup() const = 0;

        virtual void prepare() = 0;

        virtual void run() = 0;

        virtual void stop() = 0;

        virtual DeviceState getState() const = 0;

        virtual storage::Frame getData() = 0;

        virtual ~IDevice() = default;
    };

    class MockDevice : public IDevice {
    public:
        MockDevice();

        void setProfiles(const ProfileSetup &profiles, common::TimeUnit profile_length) override;

        void setDuration(common::TimeUnit total_duration) override;

        void setReadingSampling(common::TimeUnit step, uint16_t per_points = 1) override;

        std::string getSetup() const override;

        void prepare() override;

        void run() override;

        void stop() override;

        virtual DeviceState getState() const override;

        storage::Frame getData() override;

        virtual ~MockDevice() = default;

    private:
        // for silent turn off after overdue
        mutable DeviceState state_ = DeviceState::CanSet;
        mutable DeviceTimer timer_;
        double reading_sampling_ = 1;
        uint32_t returned_points_ = 0;

        void checkState(DeviceState required);

        void checkDue() const;
    };


    class DaqboardDevice : public IDevice {
    public:
        void setProfiles(const ProfileSetup &, common::TimeUnit profile_length) override;

        void setDuration(common::TimeUnit total_duration) override;

        void setReadingSampling(common::TimeUnit step, uint16_t per_points = 1) override;

        std::string getSetup() const override;

        void prepare() override;

        void run() override;

        void stop() override;

        virtual DeviceState getState() const override;

        storage::Frame getData() override;

        virtual ~DaqboardDevice() = default;

    private:
        mutable bool is_running_ = false;
        DeviceTimer timer_;
        ProfileSetup profiles_;
    };
}
#endif//UCAL2_DEVICE_DEVICE_H