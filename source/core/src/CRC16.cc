#include "CRC16.hh"

namespace GRAMSBalloon {

uint16_t calcCRC16(const std::vector<uint8_t>& pbuffer) {
  uint16_t crc = 0;  //  CCIT版は、0xffff
  const int size = pbuffer.size();
  for (int i=0; i<size; i++) {
    crc ^= pbuffer[i];
    for (int j=0; j<8; j++) {
      if (crc & 1) {
        crc = (crc >> 1) ^ 0x8408;
      }
      else {
        crc >>= 1;
      }
    }
  }
  return crc;   //  CCIC 版は、~crc
}

} /* namespace GRAMSBalloon */
