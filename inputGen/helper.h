#ifndef HELPER_H
#define HELPER_H

#include "QFPHelpers.hpp"

#include <type_traits>
#include <random>
#include <stdexcept>

#include <cassert>

void printTestVal(const std::string &funcName, float val);
void printTestVal(const std::string &funcName, double val);
void printTestVal(const std::string &funcName, long double val);

std::mt19937_64& randGenerator();
unsigned __int128 randGenerate128();

template <typename T>
auto ulpsBetween(T low, T high) -> decltype(QFPHelpers::as_uint(low)) {
  Q_UNUSED(low);
  Q_UNUSED(high);
  T zero(0.0);
  // Check for something other than 0 <= low < high
  if (low == high) {
    return zero;
  } else if (low > high) {
    return ulpsBetween(high, low);
  } else if (low < zero && high < zero) {
    return ulpsBetween(-high, -low);
  } else if (low < zero) {
    return ulpsBetween(zero, low) + ulpsBetween(zero, high);
  }

  // Now we know we have 0 <= low < high
  return QFPHelpers::as_uint(high) - QFPHelpers::as_uint(low);
}

template <typename T>
T randGenerate(T low, T high) {
  std::uniform_int_distribution<T> distribution(low, high);
  return distribution(randGenerator());
}

template <> inline
unsigned __int128 randGenerate<unsigned __int128>(unsigned __int128 low, unsigned __int128 high) {
  auto diff = high - low;
  const uint64_t zero(0);
  const uint64_t upperDiff = static_cast<uint64_t>(diff >> 64);
  const uint64_t lowerDiff = static_cast<uint64_t>(diff);
  const uint64_t upperRand = randGenerate(zero, upperDiff);
  const uint64_t lowerRand = randGenerate(zero, lowerDiff);
  unsigned __int128 randDiff = upperRand;
  randDiff = (randDiff << 64) + lowerRand;
  return low + randDiff;
}

template <typename T>
T randFpFloatGenerator(T randMin, T randMax) {
  assert(!isnan(randMin));
  assert(!isnan(randMax));
  assert(randMin <= randMax);

  const auto ulps = ulpsBetween<T>(randMin, randMax);
  const decltype(ulps) zero(0);
  const auto randUlps = randGenerate(zero, ulps);

  const auto randMinAsInt = QFPHelpers::as_int(randMin);

  T retval = QFPHelpers::as_float(
      static_cast<decltype(randMinAsInt)>(randUlps) + randMinAsInt);
  assert(retval >= randMin);
  assert(retval <= randMax);
  return retval;
}

template <typename T>
T randRealFloatGenerator(T randMin, T randMax) {
  assert(!isnan(randMin));
  assert(!isnan(randMax));
  assert(randMin <= randMax);

  auto& gen = randGenerator();
  std::uniform_real_distribution<T> distribution(randMin, randMax);
  return distribution(gen);
}

enum class RandType {
  UniformFP,    // uniform on the space of Floating-Point(FP) numbers
  UniformReals, // uniform on the real number line, then projected to FP
};

template <typename T>
T generateFloatBits(T randMin, T randMax,
                    RandType type = RandType::UniformFP) {
  switch (type) {
    case RandType::UniformFP:
      return randFpFloatGenerator<T>(randMin, randMax);
    case RandType::UniformReals:
      return randRealFloatGenerator<T>(randMin, randMax);
    default:
      throw std::runtime_error("Unimplemented rand type");
  }
}

enum class RandomFloatType {
  Positive,
  Negative,
  Any,
};

template <typename T>
T generateRandomFloat(T randMin, T randMax,
                      RandomFloatType fType = RandomFloatType::Any,
                      RandType rType = RandType::UniformFP) {
  static_assert(
      std::is_floating_point<T>::value,
      "generateRandomFloats() can only be used with floating point types"
      );

  // Generate a random floating point number
  T val;
  do {
    val = generateFloatBits<T>(randMin, randMax, rType);
  } while (isnan(val));

  // Convert the values based on the desired qualities
  if (fType == RandomFloatType::Positive) {
    val = std::abs(val);
  } else if (fType == RandomFloatType::Negative) {
    val = -std::abs(val);
  } else if (fType == RandomFloatType::Any) {
    // Do nothing
  } else {
    throw std::runtime_error("Unsupported RandomFloatType passed in");
  }

  return val;
}

#endif // HELPER_H
