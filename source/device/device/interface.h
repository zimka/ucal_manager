#ifndef UCAL2_DEVICE_INTERFACE_H
#define UCAL2_DEVICE_INTERFACE_H
#include "common/measures.h"
#include "common/keys.h"

#include "storage/frame.h"
#include "storage/signal.h"

namespace device{
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
}
#endif//UCAL2_DEVICE_INTERFACE_H