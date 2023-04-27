#ifndef ErrorManager_H
#define ErrorManager_H 1


#include <iostream>
#include <vector>

/**
 * A class to handle software error
 *
 * @author Tsubasa Tamba,
 * @date 2023-04-27
 */

namespace gramsballoon {

enum class ErrorType {
  INVALID_COMMAND = 0
};

class ErrorManager
{
public:
  ErrorManager();
  void resetError();
  void setError(ErrorType v);
  
  uint64_t ErrorCode() { return errorCode_; }

private:
  uint64_t errorCode_ = 0;
};

} /* namespace gramsballoon */

#endif /* ErrorManager_H */
