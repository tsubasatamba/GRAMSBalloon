%module GRAMSBalloon
%{
// include headers of my modules
#include <anlnext/BasicModule.hh>
#include "SimpleLoop.hh"
#ifdef USE_PIGPIO
#include "SPIManager.hh"
#endif
#ifdef USE_PIGPIO
#include "GetEnvironmentalData.hh"
#endif
#ifdef USE_WAVEFORMS
#include "ReceiveCommand.hh"
#endif
#ifdef USE_WAVEFORMS
#include "AnalogDiscoveryManager.hh"
#endif
#ifdef USE_WAVEFORMS
#include "ControlHighVoltage.hh"
#endif
#ifdef USE_PIGPIO
#include "MeasureTemperatureWithRTDSensor.hh"
#endif
#ifdef USE_WAVEFORMS
#include "ReadWaveform.hh"
#endif
#ifdef USE_WAVEFORMS
#include "SendTelemetry.hh"
#endif
#include "GetRaspiStatus.hh"
#ifdef USE_ICM20948
#include "MeasureAcceleration.hh"
#endif
#include "ReceiveTelemetry.hh"
%}

%include "std_vector.i"
%import(module="anlnext/ANL") "anlnext/ruby/ANL.i"

// interface to my modules

namespace gramsballoon {

class SimpleLoop : public anlnext::BasicModule
{
public:
  SimpleLoop();
};

#ifdef USE_PIGPIO
class SPIManager : public anlnext::BasicModule
{
public:
  SPIManager();
};
#endif

#ifdef USE_PIGPIO
class GetEnvironmentalData : public anlnext::BasicModule
{
public:
  GetEnvironmentalData();
};
#endif

#ifdef USE_WAVEFORMS
class ReceiveCommand : public anlnext::BasicModule
{
public:
  ReceiveCommand();
};
#endif

#ifdef USE_WAVEFORMS
class AnalogDiscoveryManager : public anlnext::BasicModule
{
public:
  AnalogDiscoveryManager();
};
#endif

#ifdef USE_WAVEFORMS
class ControlHighVoltage : public anlnext::BasicModule
{
public:
  ControlHighVoltage();
};
#endif

#ifdef USE_PIGPIO
class MeasureTemperatureWithRTDSensor : public anlnext::BasicModule
{
public:
  MeasureTemperatureWithRTDSensor();
};
#endif

#ifdef USE_WAVEFORMS
class ReadWaveform : public anlnext::BasicModule
{
public:
  ReadWaveform();
};
#endif

#ifdef USE_WAVEFORMS
class SendTelemetry : public anlnext::BasicModule
{
public:
  SendTelemetry();
};
#endif

class GetRaspiStatus : public anlnext::BasicModule
{
public:
  GetRaspiStatus();
};

#ifdef USE_ICM20948
class MeasureAcceleration :  public anlnext::BasicModule
{
public:
  MeasureAcceleration();
};
#endif

class ReceiveTelemetry :  public anlnext::BasicModule
{
public:
  ReceiveTelemetry();
};

} // namespace GRAMSBalloon
