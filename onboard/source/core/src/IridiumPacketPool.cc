#include "IridiumPacketPool.hh"
namespace gramsballoon::pgrams {
void IridiumPacketPool::push(const std::shared_ptr<CommunicationFormat> &packet, bool overwrite) {
  if (packet == nullptr) {
    std::cout << "IridiumPacketPool::push: packet is nullptr." << std::endl;
    return;
  }
  if (overwrite) {
    overwrittenPacket_ = packet;
    overwrittenPacketIndex_ = 0;
    return;
  }
  packetQueue_.push(packet);
  overwrittenPacketIndex_++;
}
void IridiumPacketPool::pop() {
  if (overwrittenPacket_ && overwrittenPacketIndex_ == 0) {
    overwrittenPacket_ = nullptr;
    overwrittenPacketIndex_ = 0;
    return;
  }
  if (!packetQueue_.empty()) {
    packetQueue_.pop();
    if (overwrittenPacketIndex_ > 0) {
      overwrittenPacketIndex_--;
    }
  }
}
} // namespace gramsballoon::pgrams