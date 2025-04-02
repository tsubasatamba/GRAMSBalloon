#ifndef GB_MQTTFormatConverter_hh
#define GB_MQTTFormatConverter_hh 1
#include "CommunicationFormat.hh"
#include <memory>
#define GB_MQTTFormatConverter_ALL_BINARY 1
//#undef GB_MQTTFormatConverter_ALL_BINARY
namespace gramsballoon::pgrams {
/// Function to convert item to string
template <typename T>
static inline std::vector<uint8_t> DivideArray(const T &item) {
  static_assert(std::is_integral<T>::value, "T must be an integral type");
  constexpr size_t size = sizeof(T);
  std::vector<uint8_t> result(size);
  for (size_t i = 0; i < size; ++i) {
    result[i] = static_cast<uint8_t>((item >> (8 * (size - 1 - i))) & 0xFF);
  }
  return result;
}
template <class T>
struct ConvertItemWrapper {
  static std::string ConvertItem(const T &item) {
#ifdef GB_MQTTFormatConverter_ALL_BINARY
    if constexpr (std::is_integral<T>::value) {
      std::string result = "";
      auto arr = DivideArray(item);
      for (const auto &i: arr) {
        result += ConvertItemWrapper<uint8_t>::ConvertItem(i);
      }
      return result;
    }
#endif
    return std::to_string(item);
  }
};
template <class T, class... V>
struct ConvertItemWrapper<std::vector<T, V...>> {
  static std::string ConvertItem(const std::vector<T, V...> &item) {
    std::string result = "";
    for (const auto &i: item) {
      result += ConvertItemWrapper<T>::ConvertItem(i);
    }
    return result;
  }
};
template <class T>
struct ConvertItemWrapper<std::initializer_list<T>> {
  static std::string ConvertItem(const std::initializer_list<T> &item) {
    std::string result = "";
    for (const auto &i: item) {
      result += ConvertItemWrapper<T>::ConvertItem(i);
    }
    return result;
  }
};
class MQTTFormatConverter {
public:
  MQTTFormatConverter();
  ~MQTTFormatConverter() = default;

private:
  MQTTFormatConverter(const MQTTFormatConverter &r) = default;

public:
  std::string Convert(time_t t, const CommunicationFormat &comdef);

private:
  template <typename T>
  std::string ConstructOneItem(const std::string &key, const T &item) {
    std::string result;
    result = "\"" + key + "\":" + ConvertItemWrapper<T>::ConvertItem(item);
    return result;
  }
};

template <>
inline std::string ConvertItemWrapper<std::string>::ConvertItem(const std::string &item) {
  return "\"" + item + "\"";
}
template <>
inline std::string ConvertItemWrapper<uint8_t>::ConvertItem(const uint8_t &item) {
  return std::string(1, static_cast<char>(item));
}
template <>
inline std::string ConvertItemWrapper<int32_t>::ConvertItem(const int32_t &item) {
  return ConvertItemWrapper<std::initializer_list<uint8_t>>::ConvertItem({static_cast<uint8_t>((item >> 24) & 0xFF), static_cast<uint8_t>((item >> 16) & 0xFF), static_cast<uint8_t>((item >> 8) & 0xFF), static_cast<uint8_t>(item & 0xFF)});
}
} // namespace gramsballoon::pgrams
#endif //GB_MQTTFormat_hh