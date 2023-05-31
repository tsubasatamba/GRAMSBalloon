#include "RunIDManager.hh"
#include "DateManager.hh"
#include <fstream>

using namespace anlnext;

namespace gramsballoon {

RunIDManager::RunIDManager()
{
  filename_ = "/home/grams/settings/run_id/run_id.txt";
}

RunIDManager::~RunIDManager() = default;

ANLStatus RunIDManager::mod_define()
{
  define_parameter("filename", &mod_class::filename_);
  define_parameter("chatter", &mod_class::chatter_);

  return AS_OK;
}

ANLStatus RunIDManager::mod_initialize()
{
  timeStampStr_ = getTimeStr();
  
  std::ifstream ifs(filename_);
  std::string s = "";
  int id = -1;
  while (ifs >> id >> s) {
    continue;
  }
  runID_ = id + 1;
  ifs.close();

  std::ofstream ofs(filename_, std::ios::app);
  ofs << runID_ << " " << timeStampStr_ << "\n";
  ofs.flush();
  ofs.close();
  
  const std::string send_telem_md = "SendTelemetry";
  if (exist_module(send_telem_md)) {
    get_module_NC(send_telem_md, &sendTelemetry_);
  }

  return AS_OK;
}

ANLStatus RunIDManager::mod_analyze()
{
  return AS_OK;
}

ANLStatus RunIDManager::mod_finalize()
{
  return AS_OK;
}

} /* namespace gramsballoon */
