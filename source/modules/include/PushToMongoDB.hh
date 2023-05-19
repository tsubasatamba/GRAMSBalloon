#ifndef GRAMSBALOON_PushToMongoDB_H
#define GRAMSBALOON_PushToMongoDB_H 1

#include <anlnext/BasicModule.hh>
#include <hsquicklook/MongoDBClient.hh>
#include "InterpretTelemetry.hh"
#include "TelemetryDefinition.hh"


namespace gramsballoon {

class MongoDBClient;

class PushToMongoDB : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(PushToMongoDB, 3.0);
public:
  PushToMongoDB();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

  void pushHKTelemetry();
  void pushWFTelemetry();
  void pushStatusTelemetry();

private:
  hsquicklook::MongoDBClient* mongodbClient_ = nullptr;
  InterpretTelemetry* interpreter_ = nullptr;
  int chatter_ = 0;
};

} /* namespace gramsballoon */

#endif /* GRAMSBALLOON_PushToMongoDB_H */
