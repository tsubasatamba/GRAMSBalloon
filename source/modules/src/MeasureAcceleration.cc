#include "MeasureAcceleration.hh"

using namespace anlnext;

namespace gramsballoon
{
    MeasureAcceleration::MeasureAcceleration() = default;
    MeasureAcceleration::~MeasureAcceleration() = default;
    anlnext::ANLStatus MeasureAcceleration::mod_define()
    {
        define_parameter("device_path", &mod_class::devicePath_);
        // define_parameter("calibrateGyro", &mod_class::device)
        // define_parameter("Accel_range", &mod_class::conf_->mAcc.mRange);
        return AS_OK;
    }
    anlnext::ANLStatus MeasureAcceleration::mod_initialize()
    {

        // strcpy(conf_->mDevice, devicePath_.c_str());
        // std::cout
        //     << "MeasureAcceleration: Now Initializing...";
        // icm_->initialise(*conf_.get());

        icmIO_->initialise();
        icmIO_->calibrateGyro();
        data_ = icmIO_->getData();
        return AS_OK;
    }
    anlnext::ANLStatus MeasureAcceleration::mod_analyze()
    {
        icmIO_->measure();
        std::cout << "Accel: ";
        for (int j = 0; j < 3; j++)
        {
            std::cout << data_->mAcc[j] << " ";
        }
        std::cout << std::endl;
        std::cout << "Gyro(dps): ";
        for (int j = 0; j < 3; j++)
        {
            std::cout << data_->mGyro[j] << " ";
        }
        std::cout << std::endl;
        std::cout << "Magnet[uT]";
        for (int j = 0; j < 3; j++)
        {
            std::cout << data_->mMag[j] << " ";
        }
        std::cout << std::endl;
        return AS_OK;
    }
}