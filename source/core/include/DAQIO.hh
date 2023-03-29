#ifndef DAQIO_H
#define DAQIO_H 1


#include <iostream>
#include <vector>
#include "AnalogDiscoveryIO.hh"

#define RANDOM_TRIGGER 0
#define PERIODIC_TRIGGER 1
#define SELF_TRIGGER 2
#define EXTERNAL_TRIGGER 11
#define PERIODIC_TRIGGER_MS 500

#define TRIGGER_SLOPE_RISE 0
#define TRIGGER_SLOPE_FALL 1
#define TRIGGER_SLOPE_EITHER 2


/**
 * A class to handle DAQ
 *
 * @author Tsubasa Tamba, Masato Kimura
 * @date 2023-03-21
 */

namespace gramsballoon {

class DAQIO
{
public:
  DAQIO();
  void setAnalogDiscoveryIO(AnalogDiscoveryIO* io);
  void initialize();
  void setTriggerParameters(int device, int channel, int mode, double level, double position);
  void setSampleParameters(double freq, double tw);
  int setupTrigger();
  int getData(int event_id, std::vector<int16_t>& header, std::vector<std::vector<int16_t>>& data);
  void generateFileHeader(std::vector<int16_t>& header, int16_t num_event);
  void generateFileFooter(std::vector<int16_t>& footer);
  

  
private:
  AnalogDiscoveryIO* ADIO_;
  int trigDevice_ = 0;
  int trigChannel_ = 0;
  int trigMode_ = 2;
  int trigSrc_ = 2;
  DwfTriggerSlope trigSlope_ = 0;
  double trigLevel_ = 0.0; // volt
  double trigPosition_ = 0.0; // us

  double timeWindow_ = 0.0; // us
  double sampleFreq_ = 2.0; //MHz
  int numSample_ = 0;
  const int getDataMaxTrial_ = 1000;
};

} /* namespace gramsballoon */

#endif /* DAQIO_H */
