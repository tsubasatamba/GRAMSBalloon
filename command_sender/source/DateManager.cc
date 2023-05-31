#include "DateManager.hh"
#include <boost/format.hpp>

namespace gramsballoon
{

std::string getTimeStr()
{
    struct tm *now;
    struct timeval total_usec;
    int year, month, day, hour, min, sec;
    if (gettimeofday(&total_usec, NULL) == -1)
    {
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
    /* https://qiita.com/gitcho/items/d0b375586a9b447d28c1 */

} /* namespace gramsballoon */