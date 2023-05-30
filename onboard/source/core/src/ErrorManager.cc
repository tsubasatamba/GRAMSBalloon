#include "ErrorManager.hh"
#include <thread>
#include <chrono>
#include <sys/time.h>

namespace gramsballoon {

ErrorManager::ErrorManager()
{
}

void ErrorManager::resetError()
{
  errorCode_ = (uint64_t)0;
}

void ErrorManager::setError(ErrorType v)
{
  const int shift = static_cast<int>(v);
  const uint64_t one = 1;
  errorCode_ |= (one << shift);
}

int ErrorManager::strToBit(const std::string& s)
{
  std::optional<ErrorType> op = magic_enum::enum_cast<ErrorType>(s);
  if (!op.has_value()) {
    return -1;
  }
  ErrorType err = op.value();
  const int v = static_cast<int>(err);
  return v;
}

std::string ErrorManager::bitToStr(int v)
{
  ErrorType err = static_cast<ErrorType>(v);
  std::string_view sv = magic_enum::enum_name(err);
  if (sv.data()==nullptr) {
    return "";
  }
  std::string s(sv.data());
  return s;
}

} /* namespace gramsballoon */
