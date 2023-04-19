#include "PushToMongoDB.hh"

#include <cstdint>
#include <boost/format.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include "MongoDBClient.hh"
#include "DocumentBuilder.hh"

using namespace anlnext;

namespace gramsballoon {

PushToMongoDB::PushToMongoDB()
{
}

ANLStatus PushToMongoDB::mod_define()
{
  return AS_OK;
}

ANLStatus PushToMongoDB::mod_initialize()
{
  get_module_NC("MongoDBClient", &mongodb_client_);
  mongodb_client_->createCappedCollection("grams", 1*1024*1024);

  get_module("InterpretHKTelemetry", &hk_telemetry_);

  return AS_OK;
}

ANLStatus PushToMongoDB::mod_analyze()
{
  const TelemetryDefinition hk_info = hk_telemetry_->getHKTelemetry();

  DocumentBuilder builder("Telemetry", "HK");
  builder.setTI(0);
  builder.setTimeNow();

  {
    const std::string section_name = "Header";
    auto section = bsoncxx::builder::stream::document{}
      << "Start"           << hk_info->Start()
      << "Telemetry_Type"  << hk_info->TelemetryType()
      << "Time"            << hk_info->Time()
      << "Time_us"         << hk_info->Time_us()
      << "Telemetry_Index" << hk_info->Telemetry_Index()
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }

  {
    const std::string section_name = "Detector";
    auto section = bsoncxx::builder::stream::document{}
      << "Event_Count"                  << hk_info->Event_Count()
      << "Trigger_Count"                << hk_info->Trigger_Count()
      << "Chamber_Puressure"            << hk_info->Trigger_Count()
      << "Chamber_Temperature_1"        << hk_info->Trigger_Count()
      << "Chamber_Temperature_2"        << hk_info->Trigger_Count()
      << "Chamber_Temperature_3"        << hk_info->Trigger_Count()
      << "Valve_Temperature"            << hk_info->Trigger_Count()
      << "Outer_Temperature"            << hk_info->Trigger_Count()
      << "TPC_High_Voltage_Setting"     << hk_info->Trigger_Count()
      << "TPC_High_Voltage_Measurement" << hk_info->Trigger_Count()
      << "PMT_High_Voltage_Setting"     << hk_info->Trigger_Count()
      << "PMT_High_Voltage_Measurement" << hk_info->Trigger_Count()
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }

  {
    const std::string section_name = "DAQ_Vessel";
    auto section_stream = bsoncxx::builder::stream::document{};
    section_stream
      << "CPU_Temperature" << hk_info->CPU_Temperature();
    for (int k=1; k<=5; k++) {
      section_stream << (boost::format("HK_Temperature_%d") % k).str() << hk_info->HK_Temperatures(k);
      section_stream << (boost::format("HK_Humidity_%d") % k).str()    << hk_info->HK_Humidity(k);
      section_stream << (boost::format("HK_Pressure_%d") % k).str()    << hk_info->HK_Pressure(k);
    }
    for (int k=1; k<=9; k++) {
      section_stream << (boost::format("Acceleration_%d") % k).str() << hk_info->Acceleration(k);
    }
    section_stream
      << "Main_Current" << hk_info->MainCurrent()
      << "Main_Voltage" << hk_info->MainVoltage();
    auto section = section_stream << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }

  {
    const std::string section_name = "Software";
    auto section = bsoncxx::builder::stream::document{}
      << "Last_Command_Index"   << hk_info->Event_Count()
      << "Last_Command_Code"    << hk_info->Event_Count()
      << "Command_Reject_Count" << hk_info->Event_Count()
      << "Software_Error_Code"  << hk_info->Event_Count()
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }

  auto doc = builder.generate();
  mongodb_client_->push("grams", doc);

  return AS_OK;
}

} /* namespace hsquicklook */
