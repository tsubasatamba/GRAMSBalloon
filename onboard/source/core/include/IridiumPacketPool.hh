#ifndef GB_IRIDIUM_PACKET_POOL_HH
#define GB_IRIDIUM_PACKET_POOL_HH 1
#include "CommunicationFormat.hh"
#include <memory>
#include <queue>

namespace gramsballoon::pgrams {
class IridiumPacketPool {
public:
  IridiumPacketPool() {
    overwrittenPacket_.reset();
  }
  virtual ~IridiumPacketPool() = default;
  void push(const std::shared_ptr<CommunicationFormat> &packet, bool overwrite = false);

  void pop();
  bool empty() const {
    if (overwrittenPacket_) {
      return true;
    }
    return packetQueue_.empty();
  }
  std::shared_ptr<CommunicationFormat> front() const {
    if (overwrittenPacket_ && overwrittenPacketIndex_ == 0) {
      return overwrittenPacket_;
    }
    if (!packetQueue_.empty()) {
      return packetQueue_.front();
    }
    return nullptr;
  }
  size_t size() const {
    size_t sz = packetQueue_.size();
    if (overwrittenPacket_ && overwrittenPacketIndex_ > 0) {
      sz += 1;
    }
    return sz;
  }

private:
  std::queue<std::shared_ptr<CommunicationFormat>> packetQueue_;
  std::shared_ptr<CommunicationFormat> overwrittenPacket_;
  size_t overwrittenPacketIndex_ = 0;
};
} // namespace gramsballoon::pgrams
#endif //GB_IRIDIUM_PACKET_POOL_HH