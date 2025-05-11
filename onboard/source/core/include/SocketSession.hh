#ifndef GRAMSBalloon_SocketSession_hh
#define GRAMSBalloon_SocketSession_hh 1
#include "boost/asio.hpp"
#include "boost/bind/bind.hpp"
#include <atomic>
#include <deque>
#include <iostream>
#include <memory>
#include <mutex>
namespace gramsballoon::pgrams {
class SocketSession: public std::enable_shared_from_this<SocketSession> {
public:
  SocketSession(boost::asio::ip::tcp::socket &socket) : socket_(std::move(socket)), buftemp_(BUFFER_SIZE) {};
  virtual ~SocketSession() {
    if (socket_.is_open()) {
      socket_.close();
    }
  }

protected:
  auto &getBuffer() {
    return buffer_;
  }

  auto &getSocket() {
    return socket_;
  }
  void sendHandler() {
    if (stopFlag_.load(std::memory_order_acquire)) {
      return;
    }
    auto self = shared_from_this();
    sendMutex_.lock();
    if (sendQueue_.empty()) {
      return;
    }
    auto data = std::move(sendQueue_.front());
    sendQueue_.pop_front();
    sendMutex_.unlock();
    socket_.async_send(boost::asio::buffer(data.data(), data.size()), [this, self](const boost::system::error_code &error, std::size_t bytes_transferred) {
      if (error) {
        std::cerr << "Error in SocketSession: Send failed. " << error.message() << std::endl;
        stopFlag_.store(true, std::memory_order_release);
        socket_.close();
        std::cerr << "Socket closed." << std::endl;
        return;
      }
      on_send(error, bytes_transferred);
      sendHandler();
    });
  }
  void receiveHandler() {
    if (stopFlag_.load(std::memory_order_acquire)) {
      return;
    }
    auto self = shared_from_this();
    buftemp_.resize(BUFFER_SIZE);
    socket_.async_receive(boost::asio::buffer(buftemp_.data(), buftemp_.size()), [this, self](const boost::system::error_code &error, std::size_t bytes_transferred) {
      if (error) {
        std::cerr << "Error in SocketSession: Receive failed. " << error.message() << std::endl;
        return;
      }
      {
        std::lock_guard<std::mutex> lock(mutex_);
        buffer_.push_back(buftemp_);
      }
      on_receive(error, bytes_transferred);
      receiveHandler();
    });
  }
  virtual void on_send(const boost::system::error_code &error, std::size_t bytes_transferred);
  virtual void on_receive(const boost::system::error_code &error, std::size_t bytes_transferred);

private:
  static constexpr size_t BUFFER_SIZE = 1024;
  int numTrial_ = 10;
  boost::asio::ip::tcp::socket socket_;
  std::atomic<bool> stopFlag_{false};
  std::deque<std::vector<uint8_t>> buffer_;
  std::vector<uint8_t> buftemp_;
  std::deque<std::vector<uint8_t>> sendQueue_;
  std::mutex mutex_;
  std::mutex sendMutex_;

public:
  const auto &getBuffer() const {
    return buffer_;
  }
  bool isBufferEmpty() {
    bool empty = false;
    {
      std::lock_guard<std::mutex> lock(mutex_);
      empty = buffer_.empty();
    }
    return empty;
  }
  bool getStopFlag() const {
    return stopFlag_.load(std::memory_order_acquire);
  }
  virtual void start() {}
  auto getMutex() {
    return &mutex_;
  }
  void stop() {
    stopFlag_.store(true, std::memory_order_release);
    socket_.close();
  }
  void requestSend(const void *buf, size_t n) {
    if (stopFlag_.load(std::memory_order_acquire)) {
      return;
    }
    std::lock_guard<std::mutex> lock(sendMutex_);
    sendQueue_.emplace_back(static_cast<const uint8_t *>(buf), static_cast<const uint8_t *>(buf) + n);
  }

  template <typename T>
  void requestSend(const std::vector<T> &data) {
    return requestSend(data.data(), data.size() * sizeof(T));
  }
};
} // namespace gramsballoon::pgrams
#endif // GRAMSBalloon_SocketSession_hh