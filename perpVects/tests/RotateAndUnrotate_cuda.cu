#include <cuda.h>

template <typename T>
class RotateAndUnrotate(std::string id): public CudaBase{
public:
  __device__
    void runKernel(QFPTest::testInput const *ti,
		   cudaResultType const *rt){
    
  }
}

REGISTER_TYPE(RotateAndUnrotateCU)
