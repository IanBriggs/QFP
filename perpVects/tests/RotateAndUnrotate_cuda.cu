#include <cuda.h>
#include "../testBase.h"
//#include "../QFPHelpers.h"
#include "CudaBase.hpp"
#include <stdio.h> //del me later -- just testing kernel

template <typename T>
class RotateAndUnrotateCU(std::string id): public CudaBase{
public:
  __device__
    void runKernel(QFPTest::testInput const *ti,
		   cudaResultType const *rt){
    printf("hi from runKernel: RotateAndUnrotate");
    ti->keys = {"RotateAndUnrotateCU", "test1"};
    ti->values = {0.123, 4.567};
  }
}

REGISTER_TYPE(RotateAndUnrotateCU)
