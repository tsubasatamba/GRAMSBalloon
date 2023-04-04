#include "ICM20948IO.hh"

ICM20948IO::ICM20948IO()
{
    ICM20948();
    strcpy(conf_.mDevice, "/dev/i2c-1");
}
void ICM20948IO::setConf(ICM20948::Config &conf)
{
    conf_ = conf;
}

bool ICM20948IO::initialise()
{
    return initialise(conf_);
}
