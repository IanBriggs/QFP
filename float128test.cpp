#include<iostream>
//#include<type_traits>
#include<typeinfo>

using namespace std;

int
main(){
  cout << "checking __float128" << endl;
  cout << "typeid(__float128).name(): " << typeid(__float128).name() << endl;
  typedef std::conditional<std::is_floating_point<__float128>::value, float, int>::type x;
  cout << "is_floating_point says: " << typeid(x).name() << endl;
}
