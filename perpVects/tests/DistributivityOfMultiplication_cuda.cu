#include <cuda.h>
#include <thrust/device_vector>

template <typename T>
class DistributivityOfMultiplicationCU: public CudaBase{
}

REGISTER_TYPE(DistributivityOfMultiplicationCU)
