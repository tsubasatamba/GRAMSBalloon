#include "ICM20948IO.hh"

ICM20948IO::ICM20948IO()
{
    currentTime_ = getTime();
    devicePath_ = "/dev/i2c-1";
}
double ICM20948IO::getTime()
{
    struct timespec timeStruct;
    clock_gettime(CLOCK_REALTIME, &timeStruct);
    return static_cast<double>(timeStruct.tv_sec) + static_cast<double>(timeStruct.tv_nsec / 1000000) / 1000;
}

const IMUData *ICM20948IO::getData()
{
    return data_;
}

void ICM20948IO::wait_for_process()
{
    int sleepTime = static_cast<int>(((1.0f / icm_.getConfig().mFramerate) - (getTime() - currentTime_)) * 1000000.0);
    if (sleepTime > 0)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(sleepTime));
    }
}

/**
 * @brief Calibrates the gyroscope by averaging 1024 samples of gyro and uploading them as a bias to the device.
 * @note Please make sure to be stationary!
 */
void ICM20948IO::calibrateGyro()
{
    icm_.calibrateGyro();
}

void ICM20948IO::setDevicePath(std::string &devicePath)
{
    devicePath = devicePath_;
}

bool ICM20948IO::initialise()
{
    strcpy(conf_.mDevice, devicePath_.c_str());
    bool rslt = icm_.initialise(conf_);
    if (!rslt)
    {
        std::cerr << "Device not found" << std::endl;
    }
    return rslt;
}

bool ICM20948IO::measure()
{
    data_ = &icm_.imuDataGet();
    wait_for_process();
    return true;
}
