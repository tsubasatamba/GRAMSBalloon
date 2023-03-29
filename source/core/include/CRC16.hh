#ifndef CRC16_H
#define CRC16_H 1


#include <iostream>
#include <vector>

namespace GRAMSBalloon {

uint16_t calcCRC16(const std::vector<uint8_t>& pbuffer);

} /* namespace GRAMSBalloon */

#endif /* CRC16_H */
