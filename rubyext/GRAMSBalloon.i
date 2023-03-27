%module GRAMSBalloon
%{
// include headers of my modules
#include <anlnext/BasicModule.hh>
#include "SimpleLoop.hh"
#include "SPIManager.hh"
#include "GetEnvironmentalData.hh"
#include "ReceiveCommand.hh"
#include "AnalogDiscoveryManager.hh"
#include "ControlHighVoltage.hh"
#include "MeasureTemperatureWithRTDSensor.hh"
#include "ReadWaveform.hh"
#include "SendTelemetry.hh"
#include "GetRaspiStatus.hh"
%}

%include "std_vector.i"
%import(module="anlnext/ANL") "anlnext/ruby/ANL.i"

// interface to my modules

namespace GRAMSBalloon {

class SimpleLoop : public anlnext::BasicModule
{
public:
  SimpleLoop();
};

class SPIManager : public anlnext::BasicModule
{
public:
  SPIManager();
};

class GetEnvironmentalData : public anlnext::BasicModule
{
public:
  GetEnvironmentalData();
};

class ReceiveCommand : public anlnext::BasicModule
{
public:
  ReceiveCommand();
};

class AnalogDiscoveryManager : public anlnext::BasicModule
{
public:
  AnalogDiscoveryManager();
};

class ControlHighVoltage : public anlnext::BasicModule
{
public:
  ControlHighVoltage();
};

class MeasureTemperatureWithRTDSensor : public anlnext::BasicModule
{
public:
  MeasureTemperatureWithRTDSensor();
};

class ReadWaveform : public anlnext::BasicModule
{
public:
  ReadWaveform();
};

class SendTelemetry : public anlnext::BasicModule
{
public:
  SendTelemetry();
};

class GetRaspiStatus : public anlnext::BasicModule
{
public:
  GetRaspiStatus();
};


} // namespace GRAMSBalloon