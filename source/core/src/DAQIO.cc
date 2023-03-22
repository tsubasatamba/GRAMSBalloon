#include "DAQIO.hh"
#include <thread>
#include <chrono>
#include <sys/time.h>

DAQIO::DAQIO()
{
}

void DAQIO::setAnalogDiscoveryIO(AnalogDiscoveryIO* io)
{
  ADIO_ = io;
}

void DAQIO::initialize()
{
  const int num_devices = ADIO_ -> NumDevices();
  const std::vector<HDWF>& handler_list = ADIO_ -> HandlerList();
  for (int i=0; i<num_devices; i++) {
    FDwfAnalogInTriggerAutoTimeoutSet(handler_list[i], 0);
  }
}

void DAQIO::setTriggerParameters(int device, int channel, int mode, double level, double position)
{
  trigDevice_ = device;
  trigChannel_ = channel;
  trigMode_ = mode;
  trigSrc_ = mode & 0xf;
  trigSlope_ = (mode & 0xf0) >> 4;
  trigLevel_ = level; // volt
  trigPosition_ = position; //us
}

void DAQIO::setSampleParameters(double freq, double tw)
{
  sampleFreq_ = freq; // MHz
  timeWindow_ = tw; // us
  numSample_ = static_cast<int>(freq*tw);
}

int DAQIO::setupTrigger()
{
  const int num_devices = ADIO_ -> NumDevices();
  const std::vector<HDWF>& handler_list = ADIO_ -> HandlerList();

  if (trigSrc_==RANDOM_TRIGGER) {
    FDwfDeviceTriggerSet(handler_list[trigDevice_], 0, trigsrcNone);
    for(int i=0; i<num_devices; i++) {
      if(i==trigDevice_) {
        FDwfAnalogInTriggerSourceSet(handler_list[i], trigsrcNone); // fire the trigger from the master device
      }
      else {
        FDwfAnalogInTriggerSourceSet(handler_list[i], trigsrcExternal1); // slave device is triggerd by T1 port
      }
    }
  }
  else if (trigSrc_==PERIODIC_TRIGGER) {
    for(int i=0; i<num_devices; i++) {
      FDwfAnalogInTriggerSourceSet(handler_list[i], trigsrcPC);
      FDwfDeviceTriggerSet(handler_list[i], 0, trigsrcPC);
    }
  }
  else if (trigSrc_==SELF_TRIGGER) {
    for(int i=0; i<num_devices; i++) {
      if (i==trigDevice_) {
	      FDwfAnalogInTriggerSourceSet(handler_list[i], trigsrcDetectorAnalogIn);
        FDwfAnalogInTriggerTypeSet(handler_list[i], trigtypeEdge);
        FDwfDeviceTriggerSet(handler_list[i], 0, trigsrcDetectorAnalogIn);
        for (int j=0; j<2; j++) {
          if (j==trigChannel_) {
            FDwfAnalogInTriggerChannelSet(handler_list[i], j);
            FDwfAnalogInTriggerLevelSet(handler_list[i], trigLevel_); // Volt
            if (trigSlope_==TRIGGER_SLOPE_RISE) {
              FDwfAnalogInTriggerConditionSet(handler_list[i], DwfTriggerSlopeRise);
            }
            else if (trigSlope_==TRIGGER_SLOPE_FALL) {
              FDwfAnalogInTriggerConditionSet(handler_list[i], DwfTriggerSlopeFall);
            }
            else if (trigSlope_==TRIGGER_SLOPE_EITHER){
              FDwfAnalogInTriggerConditionSet(handler_list[i], DwfTriggerSlopeEither);
            }
            else {
              std::cerr << "Trigger slope not set appropriately...: trigger slope = " << trigSlope_ << std::endl;
              return -1;
            }
          }
	      }
      }
      else {
        FDwfAnalogInTriggerSourceSet(handler_list[i], trigsrcExternal1);
      }
    }
  }
  else if (trigSrc_==EXTERNAL_TRIGGER) {
    for(int i=0; i<num_devices; i++){
      FDwfAnalogInTriggerSourceSet(handler_list[i], trigsrcExternal1);
    }
  }
  else {
    std::cerr << "Trigger source not set appropriately...: trigger src = " << trigSrc_ << std::endl;
    return -1;
  }

  for (int i=0; i<num_devices; i++){  
    for(int j=0; j<2; j++){
      const double us = 1.0E-6;
	    FDwfAnalogInTriggerPositionSet(handler_list[i], (timeWindow_*0.5-trigPosition_)*us);
    }
  }

  for (int i=0; i<num_devices; i++){  
    const bool reconfigure = false;
    const bool data_aquisition = true;  
    FDwfAnalogInConfigure(handler_list[i], reconfigure, data_aquisition);
  }
	
  std::this_thread::sleep_for(std::chrono::seconds(2));

  return 0;
}

int DAQIO::getData(int event_id, std::vector<short>& header, std::vector<std::vector<short>>& data)
{
  const int num_devices = ADIO_ -> NumDevices();
  const int num_channels = num_devices * 2;
  const std::vector<HDWF>& handler_list = ADIO_ -> HandlerList();
  STS status = 0;
  int count = 0;
  bool data_acquired = false;
  timeval event_time;

  const int sz_header = 5;
  header.resize(sz_header, 0);
  header[0] = static_cast<short>(event_id);
  data.resize(num_channels, std::vector<short>(numSample_));

  while (true) {
    FDwfAnalogInStatus(handler_list[trigDevice_], true, &status);
    if (status==DwfStateDone) {
      data_acquired = true;
      gettimeofday(&event_time, NULL);
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    if (trigSrc_==PERIODIC_TRIGGER) {
      std::this_thread::sleep_for(std::chrono::milliseconds(PERIODIC_TRIGGER_MS));
      for (int i=0; i<num_devices; i++) {
        FDwfDeviceTriggerPC(handler_list[i]);
      }
    }
    count++;
    if (count>=getDataMaxTrial_) {
      std::cout << "Data aquisition tried " << count << " times, but could not detect any event." << std::endl;
      std::cout << "Maybe trigger level is too high: trigger level = " << trigLevel_ << std::endl;
      break;
    }
  }

  if (!data_acquired) {
    std::cout << "DAQ did not detect any events." << std::endl;
    return 0;
  }

  for (int i=0; i<num_devices; i++) {
    if (i!=trigDevice_) {
      FDwfAnalogInStatus(handler_list[i], true, &status);
    }
    if (status!=DwfStateDone) {
      std::cout << "device " << i << " does not detect an event..." << std::endl;
      return -1;
    }
  }

  header[1] = static_cast<short>(event_time.tv_sec&(0xffff));
  header[2] = static_cast<short>((event_time.tv_sec>>16)&(0xffff));
  header[3] = static_cast<short>(event_time.tv_usec&(0xffff));
  header[4] = static_cast<short>((event_time.tv_usec>>16)&(0xffff));

  for (int i=0; i<num_devices; i++) {
    for (int j=0; j<2; j++) {
      const int k = i*2 + j;
      FDwfAnalogInStatusData16(handler_list[i], j, &data[k][0], 0, numSample_);
    }
  }

  return 0;
}

void DAQIO::generateFileHeader(std::vector<short>& header, short num_event, const std::vector<double>& range, const std::vector<double>& offset)
{
  const int sz_header = 32;
  header.resize(sz_header);
  timeval time_now;
  gettimeofday(&time_now, NULL);

  header[0] = static_cast<short>(trigDevice_);
  header[1] = static_cast<short>(trigChannel_);
  header[2] = static_cast<short>(trigSrc_);
  header[3] = static_cast<short>(trigSlope_);
  header[4] = static_cast<short>(trigLevel_*100);
  header[5] = static_cast<short>(trigPosition_);
  header[6] = static_cast<short>(sampleFreq_);
  header[7] = static_cast<short>(timeWindow_);
  header[8] = static_cast<short>(numSample_);
  header[9] = static_cast<short>(num_event);

  header[10] = static_cast<short>(time_now.tv_sec&(0xffff));
  header[11] = static_cast<short>((time_now.tv_sec>>16)&(0xffff));
  header[12] = static_cast<short>(time_now.tv_usec&(0xffff));
  header[13] = static_cast<short>((time_now.tv_usec>>16)&(0xffff));

  int start_index = 14;
  for (int i=0; i<static_cast<int>(range.size()); i++) {
    const double scale = 1.0E3;
    header[start_index+i] = static_cast<short>(range[i]*scale);
  }
  start_index += 4;
  for (int i=0; i<static_cast<int>(offset.size()); i++) {
    const double scale = 1.0E3;
    header[start_index+i] = static_cast<short>(offset[i]*scale);
  }
}

void DAQIO::generateFileFooter(std::vector<short>& footer)
{
  const int sz_footer = 16;
  footer.resize(sz_footer);
  timeval time_now;
  gettimeofday(&time_now, NULL);
  footer[0] = static_cast<short>(time_now.tv_sec&(0xffff));
  footer[1] = static_cast<short>((time_now.tv_sec>>16)&(0xffff));
  footer[2] = static_cast<short>(time_now.tv_usec&(0xffff));
  footer[3] = static_cast<short>((time_now.tv_usec>>16)&(0xffff));
}