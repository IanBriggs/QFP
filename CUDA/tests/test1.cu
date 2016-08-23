#include<cuda.h>
#include <iostream>

#define N 128

__global__
//template <typename T>
void doTest(int* retVal){
  // retVal[threadIdx.x] = threadIdx.x;
  if(threadIdx.x == 0){
    double a = 3.36736864456782105775439872e+26;
    double b = 1.09822961058807457775616000e+23;
    double c = 1.89503425000000000000000000e+06;

    //    auto& crit = getWatchData<T>();

    double first = (a + b) * c;
    double second = (a * c) + (b * c);
    printf("first: \t%ld\n", first);
    printf("second: \t%ld\n", second);
    // auto first_int  = QFPHelpers::FPHelpers::projectType<T>(first);
    // auto second_int = QFPHelpers::FPHelpers::projectType<T>(second);
    // auto difference = first_int - second_int;
    // //crit = difference;

    // long double score = fabs(difference);
    // printf("%
  }
}

int
main(){
  int* devMem;
  cudaMalloc(&devMem, sizeof(int) * N);
  doTest<<<N, 1>>>(devMem);
  int* hostMem = (int*)malloc(sizeof(int)*N);
  cudaMemcpy(hostMem, devMem, N, cudaMemcpyDeviceToHost);
  // for(int i = 0; i < N; ++i){
  //   std::cout << i << '\t';
  // }
}
