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
  errorCode_ = 0;
}

void ErrorManager::setError(ErrorType v)
{
  const int shift = static_cast<int>(v);
  const uint64_t one = 1;
  errorCode_ |= (one << shift);
}

} /* namespace gramsballoon */
