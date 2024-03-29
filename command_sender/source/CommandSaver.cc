#include "CommandSaver.hh"
#include <time.h>
#include <sys/time.h>
#include <boost/format.hpp>
#include <sstream>

namespace gramsballoon
{

void write_command(const std::vector<uint8_t>& command, const std::string& command_name)
{
  const std::string log_filename = get_log_filename();
  std::ofstream ofs(log_filename, std::ios_base::app | std::ios_base::out);
  const int n = command.size();
  std::string time_stamp_str = get_time_stamp_str();
  ofs << std::dec << time_stamp_str << " " << command_name << " ";

  ofs << std::hex << std::uppercase;
  
  for (int i=0; i<n; i++) {
    ofs << std::setfill('0') << std::right << std::setw(2) << static_cast<int>(command[i]) << " "; 
  }
  ofs << std::nouppercase;
  ofs << std::dec << "\n";
  ofs.flush();
  ofs.close();
}

std::string get_time_stamp_str()
{
  struct tm *now;
  struct timeval total_usec;
  int year, month, day, hour, min, sec;
  if (gettimeofday(&total_usec, NULL) == -1) {
    fprintf(stderr, "gettimeofday ERRNO=%d", errno);
    return std::string("YYYYMMDDHHMMSS");
  }

  now = localtime(&total_usec.tv_sec);
  year = now->tm_year + 1900;
  month = now->tm_mon + 1;
  day = now->tm_mday;
  hour = now->tm_hour;
  min = now->tm_min;
  sec = now->tm_sec;

  std::string s = (boost::format("%04d%02d%02d%02d%02d%02d") % year % month % day % hour % min % sec).str();
  return s;
}

std::string get_log_filename()
{
  const std::string run_id_filename = "/Users/grams/settings/run_id/run_id.txt";
  std::ifstream ifs(run_id_filename);
  int run_id = 0;
  std::string time_stamp = "YYYYMMDDHHMMSS";
  while (ifs >> run_id >> time_stamp) {
    continue;
  }
  ifs.close();
  std::ostringstream run_id_sout;
  run_id_sout << std::setfill('0') << std::right << std::setw(6) << run_id;
  std::string run_id_str = run_id_sout.str();
  std::string log_filename = "/Users/grams/data/command/command_" + run_id_str + "_" + time_stamp + ".log";
  return log_filename;
  
}

}
