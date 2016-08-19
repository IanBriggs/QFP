#ifndef CUDABASE_HPP
#define CUDABSE_HPP

#include "../testBase.h"
#include "../QFPHelpers.h"
#include <string>
#include <cuda.h>
#include <thrust/device_vector.h>
#include <thrust/host_vector.h>

template <typename T>
struct cudaPair {
  T first;
  T second;
};
  
using cudaResultType = struct {
  thrust::device_vector<cudaPair<std::string>> keys;
  thrust::device_vector<cudaPair<long double>> values;
};

template <typename T>
class CudaBase: public QFPTestTestBase {
protected:
  CudaBase(std::string id) : QFPTest::TestBase(id){}
  virtual runKernel(QFPTest::testInput const *ti,
		    cudaResultType const *rt);
public:
  QFPTest::resultType operator()(const QFPTest::testInput& ti){
    QFPTest::testInput* tid;
    cudaMalloc(&tid, sizeof(QFPTest::testInput));
    cudaMemcpy(tid, &ti);
    cudaResultType res;
    runKernel<T><<<1,1>>>(tid, &res);
    cudaDeviceSynchronize();
  }
}

#endif //CUDABSE_HPP
