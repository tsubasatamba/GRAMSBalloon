#include "MeasureAcceleration.hh"

using namespace anlnext;

namespace gramsballoon
{
    MeasureAcceleration::MeasureAcceleration()
    {
        icmIO_ = std::make_shared<ICM20948IO>();
        // conf_ = std::make_shared<ICM20948::Config>();
        devicePath_ = "/dev/i2c-1";
    };
    MeasureAcceleration::~MeasureAcceleration() = default;
    anlnext::ANLStatus MeasureAcceleration::mod_define()
    {
        define_parameter("device_path", &mod_class::devicePath_);
        // define_parameter("calibrateGyro", &mod_class::device)
        // define_parameter("Accel_range", &mod_class::conf_->mAcc.mRange);
        define_parameter("calibrateGyro", &mod_class::calibrateGyro_);
        return AS_OK;
    }
    anlnext::ANLStatus MeasureAcceleration::mod_initialize()
    {

        // strcpy(conf_->mDevice, devicePath_.c_str());
        // std::cout
        //     << "MeasureAcceleration: Now Initializing...";
        // icm_->initialise(*conf_.get());
        icmIO_->initialise();
        if (calibrateGyro_)
        {
            icmIO_->calibrateGyro();
        }
        return AS_OK;
    }
    anlnext::ANLStatus MeasureAcceleration::mod_analyze()
    {
        icmIO_->measure();
        const IMUData *data = icmIO_->getData();
        std::cout << "Accel: ";
        for (int j = 0; j < 3; j++)
        {
            std::cout << data->mAcc[j] << " ";
        }
        std::cout << std::endl;
        std::cout << "Gyro(dps): ";
        for (int j = 0; j < 3; j++)
        {
            std::cout << data->mGyro[j] << " ";
        }
        std::cout << std::endl;
        std::cout << "Magnet[uT]";
        for (int j = 0; j < 3; j++)
        {
            std::cout << data->mMag[j] << " ";
        }
        std::cout << std::endl;
        return AS_OK;
    }
    anlnext::ANLStatus MeasureAcceleration::mod_finalize()
    {
        return AS_OK;
    }
}