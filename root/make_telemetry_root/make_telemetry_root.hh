#ifndef make_telemetry_root_HH
#define make_telemetry_root_HH 1



#define MTR_GETTER(x) x()

struct TelemetryProperty{
    std::string getterName_ = "";
    double conversionCoeff_ = 1.;
};

#endif