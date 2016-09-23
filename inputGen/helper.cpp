#include "helper.h"

#include "QFPHelpers.hpp"

#include <iostream>
#include <iomanip>

/// RAII class for restoring iostream formats
class FmtRestore {
public:
  FmtRestore(std::ios& stream) : _stream(stream), _state(nullptr) {
    _state.copyfmt(_stream);
  }
  ~FmtRestore() { _stream.copyfmt(_state); }
private:
  std::ios& _stream;
  std::ios  _state;
};

void printTestVal(const std::string &funcName, float val) {
  FmtRestore restorer(std::cout);
  Q_UNUSED(restorer);

  auto intval = QFPHelpers::as_uint(val);
  std::cout << funcName << ":     0x"
            << std::hex << std::setw(8) << std::setfill('0') << intval
            << "  "
            << std::scientific << val
            << std::endl;
}

void printTestVal(const std::string &funcName, double val) {
  FmtRestore restorer(std::cout);
  Q_UNUSED(restorer);

  auto intval = QFPHelpers::as_uint(val);
  std::cout << funcName << ":     0x"
            << std::hex << std::setw(16) << std::setfill('0') << intval
            << "  "
            << std::scientific << val
            << std::endl;
}

void printTestVal(const std::string &funcName, long double val) {
  FmtRestore restorer(std::cout);
  Q_UNUSED(restorer);

  auto intval = QFPHelpers::as_uint(val);
  uint64_t lhalf = static_cast<uint64_t>((intval >> 64)) & 0xFFFFL;
  uint64_t rhalf = static_cast<uint64_t>(intval);

  std::cout << funcName << ":     0x"
            << std::hex << std::setw(4) << std::setfill('0') << lhalf
            << std::hex << std::setw(16) << std::setfill('0') << rhalf
            << "  "
            << std::scientific << val
            << std::endl;
}

namespace {

  auto generateSeed() {
    std::random_device seedGenerator;
    return seedGenerator();
  }

}

std::mt19937_64& randGenerator() {
  static auto seed = generateSeed();
  static std::mt19937_64 generator(seed);
  return generator;
}

