%module GRAMSBalloon
%{
// include headers of my modules
#include <anlnext/BasicModule.hh>
#include "SimpleLoop.hh"
#include "SPIManager.hh"
#include "GetEnvironmentalData.hh"
#include "ReceiveCommand.hh"
#include "AnalogDiscoveryManager.hh"
#include "RegulateHighVoltage.hh"
%}

%include "std_vector.i"
%import(module="anlnext/ANL") "anlnext/ruby/ANL.i"

// interface to my modules

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

class RegulateHighVoltage : public anlnext::BasicModule
{
public:
  RegulateHighVoltage();
};

