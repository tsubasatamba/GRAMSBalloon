#ifndef GRAMSBALOON_PushToMongoDB_H
#define GRAMSBALOON_PushToMongoDB_H 1

#include <anlnext/BasicModule.hh>
#include <random>

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

private:
  MongoDBClient* mongodb_client_ = nullptr;
  std::mt19937 random_engine_;
};

} /* namespace gramsballoon */

#endif /* GRAMSBALLOON_PushToMongoDB_H */
