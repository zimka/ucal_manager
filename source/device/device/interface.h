#ifndef UCAL2_DEVICE_INTERFACE_H
#define UCAL2_DEVICE_INTERFACE_H
#include "common/measures.h"
#include "common/keys.h"

#include "storage/frame.h"
#include "storage/signal.h"
#include "timer.h"

namespace device{
	/*!
	* Acquisition device, allows configure data acquisition process and get available data.
	* Consistent configuration is up to client code.
	*/
	class AcquisitionDevice{
	public:
	    virtual bool run() =0;
	    virtual bool stop() =0;
	    virtual bool isRunning() const =0;
	    virtual storage::Frame getFrame() =0;

	    virtual bool setConstVoltage(common::ControlKey id, common::VoltUnit level) = 0;
	    virtual bool setSineVoltage(common::ControlKey id, common::VoltUnit offset, common::VoltUnit amplitude, common::TimeUnit sine_period) =0;
	    virtual bool setLinearVoltage(common::ControlKey id, common::VoltUnit start, common::VoltUnit end, common::TimeUnit signal_duration) =0;
		virtual bool setSqrtVoltage(common::ControlKey id, common::VoltUnit start, common::VoltUnit end, common::TimeUnit signal_duration) =0;
	    virtual bool setSequenceVoltage(common::ControlKey id, storage::SignalData voltage_sequence, common::TimeUnit sequence_duration) =0;

	    virtual bool setTimeout(common::TimeUnit finish) =0;
		virtual common::DeviceId getId() const =0;
	    virtual ~AcquisitionDevice() =default;
	};

	/*!
	* Mock device for tests and Linux runs.
	* State checkout, timeout and frame acquisition are mocked.
	* All frame signals are zero for any configuration.
	*/
    class MockDevice: public AcquisitionDevice{
    public:
    	MockDevice();

	    virtual bool run();
	    virtual bool stop();
	    virtual bool isRunning() const;
	    virtual storage::Frame getFrame();

		virtual bool setConstVoltage(common::ControlKey id, common::VoltUnit level);
	    virtual bool setSineVoltage(common::ControlKey id, common::VoltUnit offset, common::VoltUnit amplitude, common::TimeUnit sine_period);
	    virtual bool setLinearVoltage(common::ControlKey id, common::VoltUnit start, common::VoltUnit end, common::TimeUnit signal_duration);
		virtual bool setSqrtVoltage(common::ControlKey id, common::VoltUnit start, common::VoltUnit end, common::TimeUnit signal_duration);
	    virtual bool setSequenceVoltage(common::ControlKey id, storage::SignalData voltage_sequence, common::TimeUnit sequence_duration);

	    virtual bool setTimeout(common::TimeUnit finish);
		virtual common::DeviceId getId() const;
	    virtual ~MockDevice() =default;
    private:
    	mutable bool is_running_=false;
    	size_t points_returned_=0;
    	DeviceTimer timer_;

    };
}
#endif//UCAL2_DEVICE_INTERFACE_H