#include "testBase.h"
#include "helpers.h"
#include <cuda.h>

std::ostream& operator<<(std::ostream& os, const unsigned __int128 i) {
  uint64_t hi = i >> 64;
  uint64_t lo = (uint64_t)i;
  os << hi << lo;
  return os;
}

template <typename T>
T
mask_81_to_128(T val){
  return val;
}
typedef unsigned __int128 u128;

template<>
u128
mask_81_to_128(u128 val){
  const u128 zero = 0;
  return val & (~zero >> 48);
}

template <typename R, typename T>
R
to_uint(T val){
  auto ret =  *reinterpret_cast<R*>(&val);
  return mask_81_to_128(ret);
}

void
outputResults(const QFPTest::resultType& scores){
  for(const auto& i: scores){
    std::cout << "HOST,SWITCHES,COMPILER," << i.first.second <<
      ",us," << i.second.first << "," << to_uint<u128>(i.second.first) <<
      "," << i.second.second << "," << to_uint<u128>(i.second.second) <<
      "," << i.first.first << "," << "FILENAME" << std::endl;
  }
}


int
main(int argc, char* argv[]){
  Test::resultType scores;
  for(auto& t : TestBase::getTests()){
    auto plist = t.second->create();
    for(auto pt : plist){
      scores.push_back(*pt());
    }
  }
  outputResults(scores);
}
