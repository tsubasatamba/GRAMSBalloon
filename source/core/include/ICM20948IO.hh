/**
 * A class for ICM20948
 *
 * @author Tsubasa Tamba, Shota Arai
 * @date 2023-04-04
 */

#ifndef ICM20948IO_H
#define ICM20948IO_H 1
#include "ICM20948.h"
#include <string.h>
#include <chrono>
#include <thread>

class ICM20948IO
{
public:
    ICM20948IO();
    ~ICM20948IO() = default;

private:
    ICM20948::Config conf_;
    IMUData Data;
};

#endif /*ICM20948IO_H*/