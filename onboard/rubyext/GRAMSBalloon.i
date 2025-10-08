%module GRAMSBalloon
%{
// include headers of my modules
#include <anlnext/BasicModule.hh>
#include <string>
#include "SimpleLoop.hh"
#include "Sleep.hh"
#ifndef GB_MAC
#include "ReceiveCommand.hh"
#endif
#ifndef GB_MAC
#include "SendTelemetry.hh"
#endif
#ifdef USE_RASPISYS
#include "GetRaspiStatus.hh"
#endif
#include "ReceiveTelemetry.hh"
#ifdef USE_RASPISYS
#include "ShutdownSystem.hh"
#endif
#include "InterpretTelemetry.hh"
#ifdef USE_HSQUICKLOOK
#include "PushToMongoDB.hh"
#endif
#include "RunIDManager.hh"
#include "ReadTelemetry.hh"
#include "DumpSerial.hh"
#include "GetArduinoData.hh"
#include "GetMHADCData.hh"
#include "EncodedSerialCommunicator.hh"
#include "GetCompressorData.hh"
#include "GetPressure.hh"
#include "PressureGaugeManager.hh"
#include "MeasureTemperatureWithRTDSensor.hh"
#include "MeasureTemperatureWithRTDSensorByMHADC.hh"
#include "MeasureTemperatureWithRTDSensorByArduino.hh"


#ifdef USE_MYSQL
#include "PushToMySQL.hh"
#endif
#ifdef GB_DEMO_MODE
#include "GBBasicDemoModule.hh"
#include "ShutdownSystemDemo.hh"
#include "GetRaspiStatusDemo.hh"
#endif
#include "MosquittoManager.hh"
#include "DistributeCommand.hh"
#include "ReceiveStatusFromDAQComputer.hh"
#include "DividePacket.hh"
#include "PassTelemetry.hh"
#include "InterpretDAQFormattedTelemetry.hh"
#include "IoContextManager.hh"
#include "SendCommandToDAQComputer.hh"
%}

%include "std_vector.i"
%include "stdint.i"
%include "std_string.i"
%import(module="anlnext/ANL") "anlnext/ruby/ANL.i"

// interface to my modules

namespace gramsballoon {

class SimpleLoop : public anlnext::BasicModule
{
public:
  SimpleLoop();
};
namespace pgrams{
class Sleep : public anlnext::BasicModule
{
public:
  Sleep();
};
}
#ifdef USE_PIGPIO
class SPIManager : public anlnext::BasicModule
{
public:
  SPIManager();
};
#endif

#if defined GB_MAC || defined GB_DEMO_MODE
class ReceiveCommand : public anlnext::BasicModule
{
public:
  ReceiveCommand();
};
#endif

#ifdef USE_RASPISYS
class GetRaspiStatus : public anlnext::BasicModule
{
public:
  GetRaspiStatus();
};
#endif

#if defined GB_MAC || defined GB_DEMO_MODE
class SendTelemetry : public anlnext::BasicModule
{
public:
  SendTelemetry();
};
#endif
namespace pgrams {
class ReceiveTelemetry :  public anlnext::BasicModule
{
public:
  ReceiveTelemetry();
};

class DumpSerial: public anlnext::BasicModule {
public:
    DumpSerial();
};

class GetArduinoData: public anlnext::BasicModule {
public:
  GetArduinoData();
};

class GetMHADCData: public anlnext::BasicModule {
public:
  GetMHADCData();
};

class EncodedSerialCommunicator: public anlnext::BasicModule {
public:
  EncodedSerialCommunicator();
};

class GetCompressorData: public anlnext::BasicModule {
public:
  GetCompressorData();
};

class GetPressure: public anlnext::BasicModule {
public:
  GetPressure();
};

class PressureGaugeManager: public EncodedSerialCommunicator{
public:
  PressureGaugeManager();
};
class IoContextManager: public anlnext::BasicModule{
public:
  IoContextManager();
};
class ReceiveStatusFromDAQComputer: public anlnext::BasicModule{
public:
  ReceiveStatusFromDAQComputer();
};
class SocketCommunicationManager: public anlnext::BasicModule{
public:
  SocketCommunicationManager();
};
class SendCommandToDAQComputer: public anlnext::BasicModule {
public:
  SendCommandToDAQComputer();
};
#ifdef USE_MYSQL
class PushToMySQL : public anlnext::BasicModule
{
public:
  PushToMySQL();
};
#endif
template <typename TelemType>
class MosquittoManager: public anlnext::BasicModule
{
public:
  MosquittoManager();
};

class DistributeCommand: public anlnext::BasicModule {
public:
  DistributeCommand();
};
class DividePacket: public anlnext::BasicModule {
public:
  DividePacket();
};
class PassTelemetry: public anlnext::BasicModule {
public:
  PassTelemetry();
};
template <typename TelemType>
class InterpretTelemetry : public anlnext::BasicModule
{
public:
  InterpretTelemetry();
};
} // namespace pgrams

#ifdef USE_RASPISYS
class ShutdownSystem : public anlnext::BasicModule
{
public:
  ShutdownSystem();
};
#endif


#ifdef USE_HSQUICKLOOK
class PushToMongoDB : public anlnext::BasicModule
{
public:
  PushToMongoDB();
};
#endif

class RunIDManager : public anlnext::BasicModule
{
public:
  RunIDManager();
};

class ReadTelemetry : public anlnext::BasicModule
{
public:
  ReadTelemetry();
};

#ifdef GB_DEMO_MODE
class GBBasicDemoModule : public anlnext::BasicModule
{
public:
  GBBasicDemoModule();
protected:
  double SampleFromUniformDistribution();
};

class ShutdownSystem : public anlnext::BasicModule
{
public:
  ShutdownSystem();
};

class MeasureTemperatureWithRTDSensor : public GBBasicDemoModule
{
public:
  MeasureTemperatureWithRTDSensor();
};
namespace pgrams{
class MeasureTemperatureWithRTDSensorByArduino: public MeasureTemperatureWithRTDSensor {
public:
  MeasureTemperatureWithRTDSensorByArduino();
};

class MeasureTemperatureWithRTDSensorByMHADC: public MeasureTemperatureWithRTDSensor {
public:
  MeasureTemperatureWithRTDSensorByMHADC();
};
class MeasureOrientationByMHADC: public anlnext::BasicModule {
public:
  MeasureOrientationByMHADC();
};
class InterpretDAQFormattedTelemetry: public anlnext::BasicModule {
public:
  InterpretDAQFormattedTelemetry();
};
} // namespace pgrams
class GetRaspiStatus : public GBBasicDemoModule
{
public:
  GetRaspiStatus();
};
#endif
} // namespace GRAMSBalloon
%template(TelemMosquittoManager) gramsballoon::pgrams::MosquittoManager<std::string>;
%template(ComMosquittoManager) gramsballoon::pgrams::MosquittoManager<std::vector<uint8_t>>;
%template(InterpretHKTelemetry) gramsballoon::pgrams::InterpretTelemetry<gramsballoon::pgrams::HubHKTelemetry>;