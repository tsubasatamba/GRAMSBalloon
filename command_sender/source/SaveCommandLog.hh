#ifndef SaveCommandLog_HH
#define SaveCommandLog_HH 1

#include <string>
#include <fstream>
#include <iomanip>
#include <vector>
#include "DateManager.hh"

namespace gramsballoon
{

struct CommandLog{
    std::string date = "";
    std::string commandname = "";
    std::vector<int32_t> args;
    std::vector<uint8_t> hex;
};

class SaveCommandLog
{
public:
    SaveCommandLog(std::string filename);
    ~SaveCommandLog() = default;
private:
    std::string fileName_ = "";
    std::ofstream ofs;


public:
    std::string getFileName() { return fileName_; }
    void writeCommandLog(const CommandLog &comlog);
    void close();
};
} /*namespace gramsballoon*/

#endif