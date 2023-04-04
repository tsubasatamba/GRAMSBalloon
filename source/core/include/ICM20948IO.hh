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
#include <string>
#include <iostream>

class ICM20948IO
{
public:
    ICM20948IO();
    ~ICM20948IO() = default;
    bool initialise();
    bool measure();
    static double getTime();
    const IMUData *getData();
    void wait_for_process();
    void calibrateGyro();

    void setDevicePath(std::string devicePath);

private:
    ICM20948::Config *conf_;
    const IMUData *data_;
    ICM20948 *icm_;
    double currentTime_;
    std::string devicePath_;
};

#endif /*ICM20948IO_H*/