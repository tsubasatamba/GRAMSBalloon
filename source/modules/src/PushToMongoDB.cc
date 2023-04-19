#include "PushToMongoDB.hh"

#include <cstdint>
#include <boost/format.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <hsquicklook/DocumentBuilder.hh>


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
  get_module_NC("MongoDBClient", &mongodbClient_);
  mongodbClient_->createCappedCollection("grams", 1*1024*1024);

  get_module_NC("InterpretHKTelemetry", &hkInterpreter_);

  return AS_OK;
}

ANLStatus PushToMongoDB::mod_analyze()
{
  TelemetryDefinition* hk_telemdef = hkInterpreter_->Telemdef();

  hsquicklook::DocumentBuilder builder("Telemetry", "HK");
  builder.setTI(0);
  builder.setTimeNow();

  {
    const std::string section_name = "Header";
    auto section = bsoncxx::builder::stream::document{}
      << "Start"           << 0
      << "Telemetry_Type"  << static_cast<int>(hk_telemdef->TelemetryType())
      << "Time"            << static_cast<int>((hk_telemdef->TimeNow()).tv_sec)
      << "Time_us"         << static_cast<int>((hk_telemdef->TimeNow()).tv_usec)
      << "Telemetry_Index" << static_cast<int>(hk_telemdef->TelemetryIndex())
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }

  {
    const std::string section_name = "Detector";
    auto section = bsoncxx::builder::stream::document{}
      << "Event_Count"                  << static_cast<int>(hk_telemdef->EventCount())
      << "Trigger_Count"                << static_cast<int>(hk_telemdef->TriggerCount())
      << "Chamber_Puressure"            << static_cast<int>(hk_telemdef->ChamberPressure())
      << "Chamber_Temperature_1"        << static_cast<int>(hk_telemdef->ChamberTemperature(0))
      << "Chamber_Temperature_2"        << static_cast<int>(hk_telemdef->ChamberTemperature(1))
      << "Chamber_Temperature_3"        << static_cast<int>(hk_telemdef->ChamberTemperature(2))
      << "Valve_Temperature"            << static_cast<int>(hk_telemdef->ValveTemperature())
      << "Outer_Temperature"            << static_cast<int>(hk_telemdef->OuterTemperature())
      << "TPC_High_Voltage_Setting"     << static_cast<int>(hk_telemdef->TPCHVSetting())
      << "TPC_High_Voltage_Measurement" << static_cast<int>(hk_telemdef->TPCHVMeasure())
      << "PMT_High_Voltage_Setting"     << static_cast<int>(hk_telemdef->PMTHVSetting())
      << "PMT_High_Voltage_Measurement" << static_cast<int>(hk_telemdef->PMTHVMeasure())
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }

  {
    const std::string section_name = "DAQ_Vessel";
    auto section_stream = bsoncxx::builder::stream::document{};
    section_stream
      << "CPU_Temperature" << hk_telemdef->CPUTemperature();
    for (int i=0; i<5; i++) {
      const int k = i+1;
      section_stream << (boost::format("HK_Temperature_%d") % k).str() << hk_telemdef->EnvTemperature(i);
      section_stream << (boost::format("HK_Humidity_%d") % k).str()    << hk_telemdef->EnvHumidity(i);
      section_stream << (boost::format("HK_Pressure_%d") % k).str()    << hk_telemdef->EnvPressure(i);
    }
    for (int i=0; i<3; i++) {
      const char c = 'x' + i;
      section_stream << (boost::format("Acceleration_%s") % c).str() << hk_telemdef->Acceleration(i);
      section_stream << (boost::format("Gyro_%d") % c).str() << hk_telemdef->Gyro(i);
      section_stream << (boost::format("Acceleration_%d") % c).str() << hk_telemdef->Magnet(i);
    }
    section_stream
      << "Main_Current" << static_cast<int>(hk_telemdef->MainCurrent())
      << "Main_Voltage" << static_cast<int>(hk_telemdef->MainVoltage());
    auto section = section_stream << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }

  {
    const std::string section_name = "Software";
    auto section = bsoncxx::builder::stream::document{}
      << "Last_Command_Index"   << static_cast<int>(hk_telemdef->LastCommandIndex())
      << "Last_Command_Code"    << static_cast<int>(hk_telemdef->LastCommandCode())
      << "Command_Reject_Count" << static_cast<int>(hk_telemdef->CommandRejectCount())
      << "Software_Error_Code"  << static_cast<int>(hk_telemdef->SoftwareErrorCode())
      << bsoncxx::builder::stream::finalize;
    builder.addSection(section_name, section);
  }

  auto doc = builder.generate();
  mongodbClient_->push("grams", doc);

  return AS_OK;
}

} /* namespace gramsballoon */
