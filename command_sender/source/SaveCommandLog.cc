#include "SaveCommandLog.hh"

using namespace gramsballoon;
SaveCommandLog::SaveCommandLog(std::string filename)
{
    fileName_ = filename;
    ofs.open(fileName_, std::ios_base::app | std::ios_base::out);
}

void SaveCommandLog::writeCommandLog(const CommandLog &comlog)
{
    comlog.date = getTimeStr();
    ofs << comlog.date << " " << comlog.commandname << " ";
    for (int i = 0; i < comlog.args.size();i++){
        ofs << comlog.args[i] << " ";
    }
    ofs << std::hex;
    for (int i = 0; i < comlog.hex.size(); i++)
    {
        ofs << comlog.hex[i] << " ";
    }
    ofs << std::dec << std::endl;
}

void SaveCommandLog::close(){
    ofs.close();
}