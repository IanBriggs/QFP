// This file is part of Eigen, a lightweight C++ template library
// for linear algebra.
//
// Copyright (C) 2006-2008 Benoit Jacob <jacob.benoit.1@gmail.com>
// Copyright (C) 2008 Gael Guennebaud <gael.guennebaud@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cstdlib>
#include <cerrno>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <typeinfo>

// The following includes of STL headers have to be done _before_ the
// definition of macros min() and max().  The reason is that many STL
// implementations will not work properly as the min and max symbols collide
// with the STL functions std:min() and std::max().  The STL headers may check
// for the macro definition of min/max and issue a warning or undefine the
// macros.
//
// Still, Windows defines min() and max() in windef.h as part of the regular
// Windows system interfaces and many other Windows APIs depend on these
// macros being available.  To prevent the macro expansion of min/max and to
// make Eigen compatible with the Windows environment all function calls of
// std::min() and std::max() have to be written with parenthesis around the
// function name.
//
// All STL headers used by Eigen should be included here.  Because main.h is
// included before any Eigen header and because the STL headers are guarded
// against multiple inclusions, no STL header will see our own min/max macro
// definitions.
#include <limits>
#include <algorithm>
#include <complex>
#include <deque>
#include <queue>
#include <list>
#include <map>
#include <mutex>

// To test that all calls from Eigen code to std::min() and std::max() are
// protected by parenthesis against macro expansion, the min()/max() macros
// are defined here and any not-parenthesized min/max call will cause a
// compiler error.

//QFP mods

#define EIGEN_TEST_PART_1
#define EIGEN_TEST_PART_2
#define EIGEN_TEST_PART_3

#include "split_test_helper.hpp"
#include "testBase.hpp"

extern std::map<std::string, QFPTest::ResultType> eigenResults;
extern std::mutex eigenResults_mutex;
extern std::mutex g_test_stack_mutex;


#define EIGEN_TEST_MAX_SIZE 5

#define EIGEN_NO_ASSERTION_CHECKING

//END QFP
#define min(A,B) please_protect_your_min_with_parentheses
#define max(A,B) please_protect_your_max_with_parentheses

#define FORBIDDEN_IDENTIFIER (this_identifier_is_forbidden_to_avoid_clashes) this_identifier_is_forbidden_to_avoid_clashes
// B0 is defined in POSIX header termios.h
#define B0 FORBIDDEN_IDENTIFIER


// shuts down ICC's remark #593: variable "XXX" was set but never used
#define TEST_SET_BUT_UNUSED_VARIABLE(X) X = X + 0;

#ifdef NDEBUG
#undef NDEBUG
#endif

// On windows CE, NDEBUG is automatically defined <assert.h> if NDEBUG is not defined.
#ifndef DEBUG
#define DEBUG
#endif

// bounds integer values for AltiVec
#ifdef __ALTIVEC__
#define EIGEN_MAKING_DOCS
#endif

#define DEFAULT_REPEAT 1

namespace Eigen
{
  static std::map<std::string, std::vector<std::string>> g_test_stack;
  //QFP we removed the setter (main)
  static int g_repeat = DEFAULT_REPEAT;
  static unsigned int g_seed;
  static bool g_has_set_repeat, g_has_set_seed;
}

#define EI_PP_MAKE_STRING2(S) #S
#define EI_PP_MAKE_STRING(S) EI_PP_MAKE_STRING2(S)

#define EIGEN_DEFAULT_IO_FORMAT IOFormat(4, 0, "  ", "\n", "", "", "", "")

#ifndef EIGEN_NO_ASSERTION_CHECKING

  namespace Eigen
  {
    static const bool should_raise_an_assert = false;

    // Used to avoid to raise two exceptions at a time in which
    // case the exception is not properly caught.
    // This may happen when a second exceptions is triggered in a destructor.
    static bool no_more_assert = false;
    static bool report_on_cerr_on_assert_failure = true;

    struct eigen_assert_exception
    {
      eigen_assert_exception(void) {}
      ~eigen_assert_exception() { Eigen::no_more_assert = false; }
    };
  }
  // If EIGEN_DEBUG_ASSERTS is defined and if no assertion is triggered while
  // one should have been, then the list of excecuted assertions is printed out.
  //
  // EIGEN_DEBUG_ASSERTS is not enabled by default as it
  // significantly increases the compilation time
  // and might even introduce side effects that would hide
  // some memory errors.
  #ifdef EIGEN_DEBUG_ASSERTS

    namespace Eigen
    {
      namespace internal
      {
        static bool push_assert = false;
      }
      static std::vector<std::string> eigen_assert_list;
    }
    #define eigen_assert(a)                       \
      if( (!(a)) && (!no_more_assert) )     \
      { \
        if(report_on_cerr_on_assert_failure) \
          std::cerr <<  #a << " " __FILE__ << "(" << __LINE__ << ")\n"; \
        Eigen::no_more_assert = true;       \
        throw Eigen::eigen_assert_exception(); \
      }                                     \
      else if (Eigen::internal::push_assert)       \
      {                                     \
        eigen_assert_list.push_back(std::string(EI_PP_MAKE_STRING(__FILE__) " (" EI_PP_MAKE_STRING(__LINE__) ") : " #a) ); \
      }

    #define VERIFY_RAISES_ASSERT(a)                                                   \
      {                                                                               \
        Eigen::no_more_assert = false;                                                \
        Eigen::eigen_assert_list.clear();                                                \
        Eigen::internal::push_assert = true;                                                 \
        Eigen::report_on_cerr_on_assert_failure = false;                              \
        try {                                                                         \
          a;                                                                          \
          std::cerr << "One of the following asserts should have been triggered:\n";  \
          for (uint ai=0 ; ai<eigen_assert_list.size() ; ++ai)                           \
            std::cerr << "  " << eigen_assert_list[ai] << "\n";                          \
          VERIFY(Eigen::should_raise_an_assert && # a);                               \
        } catch (Eigen::eigen_assert_exception) {                                        \
          Eigen::internal::push_assert = false; VERIFY(true);                                \
        }                                                                             \
        Eigen::report_on_cerr_on_assert_failure = true;                               \
        Eigen::internal::push_assert = false;                                                \
      }

  #else // EIGEN_DEBUG_ASSERTS
    // see bug 89. The copy_bool here is working around a bug in gcc <= 4.3
  #endif // EIGEN_DEBUG_ASSERTS

  #define EIGEN_USE_CUSTOM_ASSERT

#else // EIGEN_NO_ASSERTION_CHECKING

#define VERIFY_RAISES_ASSERT(a)

#endif // EIGEN_NO_ASSERTION_CHECKING


#define EIGEN_INTERNAL_DEBUGGING
#include <Eigen/QR>

typedef std::pair<bool, long double> cond_t;

//int vi_count = 0;

inline std::string
getFileNoEx(std::string path){
  auto lastPS = path.find_last_of("/\\");
  auto tmp = path.substr(lastPS+1);
  auto dot = tmp.find_last_of(".");
  auto retval =  tmp.substr(0,dot);
  //  std::cout << "getFileNoEx is: " << retval << " and dot is: " << dot << '\n';
  return retval;
}

inline void verify_impl(cond_t condition, const char *testname, const char *file,
                        int line, const char* /*condition_as_string*/, bool invert = false)
{
  auto fname = getFileNoEx(file);
  bool cond = condition.first != invert;
  if(eigenResults.find(fname) == eigenResults.end()){
    eigenResults_mutex.lock();
    eigenResults[fname];
    eigenResults_mutex.unlock();
  }
  auto& container = eigenResults[fname];
  container.insert({{std::string(testname) + "_" + fname + "_" + std::to_string(line), "f"},
        {(long double)cond, condition.second}});
  // std::cout << "container size is: " << container.size() << std::endl;

  // if (!condition)
  // {
  //   std::cerr << "Test " << testname << " failed in " << file << " (" << line << ")"
  //     << std::endl << "    " << condition_as_string << std::endl;
  //   std::cerr << "Stack:\n";
  //   const int test_stack_size = static_cast<int>(Eigen::g_test_stack.size());
  //   for(int i=test_stack_size-1; i>=0; --i)
  //     std::cerr << "  - " << Eigen::g_test_stack[i] << "\n";
  //   std::cerr << "\n";
  //   abort();
  // }
}

inline void verify_impl(bool condition, const char *testname, const char *file,
                        int line, const char* /*condition_as_string*/)
{
  std::string fname = getFileNoEx(file);
  if(eigenResults.find(fname) == eigenResults.end()){
    eigenResults_mutex.lock();
    eigenResults[fname];
    eigenResults_mutex.unlock();
  }
  auto& container = eigenResults[fname];
  container.insert({{std::string(testname) + "_" + fname + "_" + std::to_string(line), "f"},
        {(long double)condition, 0.0}});
}

#define VERIFY2(a) ::verify_impl(a, g_test_stack[__FILE__].back().c_str(), __FILE__, __LINE__, EI_PP_MAKE_STRING(a))

#define VERIFY2_NOT(a) ::verify_impl(a, g_test_stack[__FILE__].back().c_str(), __FILE__, __LINE__, EI_PP_MAKE_STRING(a), true)

#define VERIFY(a) ::verify_impl(a, g_test_stack[__FILE__].empty()?__FILE__:g_test_stack[__FILE__].back().c_str(), __FILE__, __LINE__, EI_PP_MAKE_STRING(a))

#define VERIFY_IS_EQUAL(a, b) VERIFY2(test_is_equal(a, b))
#define VERIFY_IS_APPROX(a, b) VERIFY2(test_isApprox(a, b))
#define VERIFY_IS_NOT_APPROX(a, b) VERIFY2_NOT(test_isApprox(a, b))
#define VERIFY_IS_MUCH_SMALLER_THAN(a, b) VERIFY2(test_isMuchSmallerThan(a, b))
#define VERIFY_IS_NOT_MUCH_SMALLER_THAN(a, b) VERIFY2_NOT(test_isMuchSmallerThan(a, b))
#define VERIFY_IS_APPROX_OR_LESS_THAN(a, b) VERIFY2(test_isApproxOrLessThan(a, b))
#define VERIFY_IS_NOT_APPROX_OR_LESS_THAN(a, b) VERIFY2_NOT(test_isApproxOrLessThan(a, b))

#define VERIFY_IS_UNITARY(a) VERIFY2(test_isUnitary(a))

#define CALL_SUBTEST(FUNC) do { \
    g_test_stack[__FILE__].push_back(EI_PP_MAKE_STRING(FUNC));  \
    FUNC; \
    g_test_stack[__FILE__].pop_back(); \
  } while (0) \


namespace Eigen {

template<typename T> inline typename NumTraits<T>::Real test_precision() { return NumTraits<T>::dummy_precision(); }
template<> inline float test_precision<float>() { return 1e-3f; }
template<> inline double test_precision<double>() { return 1e-6; }
template<> inline float test_precision<std::complex<float> >() { return test_precision<float>(); }
template<> inline double test_precision<std::complex<double> >() { return test_precision<double>(); }
template<> inline long double test_precision<long double>() { return 1e-6; }

inline cond_t test_isApprox(const int& a, const int& b)
{ return {internal::isApprox(a, b, test_precision<int>()), std::abs(a-b)}; }
inline cond_t test_isMuchSmallerThan(const int& a, const int& b)
{ return {internal::isMuchSmallerThan(a, b, test_precision<int>()), std::abs(a-b)}; }
inline cond_t test_isApproxOrLessThan(const int& a, const int& b)
{ return {internal::isApproxOrLessThan(a, b, test_precision<int>()), std::abs(a-b)}; }

inline cond_t test_isApprox(const float& a, const float& b)
{ return {internal::isApprox(a, b, test_precision<float>()), std::abs(a-b)}; }
inline cond_t test_isMuchSmallerThan(const float& a, const float& b)
{ return {internal::isMuchSmallerThan(a, b, test_precision<float>()),std::abs(a-b)}; }
inline cond_t test_isApproxOrLessThan(const float& a, const float& b)
{ return {internal::isApproxOrLessThan(a, b, test_precision<float>()), std::abs(a-b)}; }
inline cond_t test_isApprox(const double& a, const double& b)
{ return {internal::isApprox(a, b, test_precision<double>()), std::abs(a-b)}; }

inline cond_t test_isMuchSmallerThan(const double& a, const double& b)
{ return {internal::isMuchSmallerThan(a, b, test_precision<double>()),std::abs(a-b)}; }
inline cond_t test_isApproxOrLessThan(const double& a, const double& b)
{ return {internal::isApproxOrLessThan(a, b, test_precision<double>()), std::abs(a-b)}; }

inline cond_t test_isApprox(const std::complex<float>& a, const std::complex<float>& b)
{ return {internal::isApprox(a, b, test_precision<std::complex<float> >()), std::abs(a-b)}; }
inline cond_t test_isMuchSmallerThan(const std::complex<float>& a, const std::complex<float>& b)
{ return {internal::isMuchSmallerThan(a, b, test_precision<std::complex<float> >()),std::abs(a-b)}; }

inline cond_t test_isApprox(const std::complex<double>& a, const std::complex<double>& b)
{ return {internal::isApprox(a, b, test_precision<std::complex<double> >()),std::abs(a-b)}; }
inline cond_t test_isMuchSmallerThan(const std::complex<double>& a, const std::complex<double>& b)
{ return {internal::isMuchSmallerThan(a, b, test_precision<std::complex<double> >()),std::abs(a-b)}; }

inline cond_t test_isApprox(const long double& a, const long double& b)
{
    bool ret = internal::isApprox(a, b, test_precision<long double>());
    return {ret, std::abs(a-b)};
}

inline cond_t test_isMuchSmallerThan(const long double& a, const long double& b)
{ return {internal::isMuchSmallerThan(a, b, test_precision<long double>()),std::abs(a-b)}; }
inline cond_t test_isApproxOrLessThan(const long double& a, const long double& b)
{ return {internal::isApproxOrLessThan(a, b, test_precision<long double>()),std::abs(a-b)}; }

template<typename Type1, typename Type2>
inline cond_t test_isApprox(const Type1& a, const Type2& b)
{
  return {a.isApprox(b, test_precision<typename Type1::Scalar>()), 0.0};
}

// The idea behind this function is to compare the two scalars a and b where
// the scalar ref is a hint about the expected order of magnitude of a and b.
// WARNING: the scalar a and b must be positive
// Therefore, if for some reason a and b are very small compared to ref,
// we won't issue a false negative.
// This test could be: abs(a-b) <= eps * ref
// However, it seems that simply comparing a+ref and b+ref is more sensitive to true error.
template<typename Scalar,typename ScalarRef>
inline cond_t test_isApproxWithRef(const Scalar& a, const Scalar& b, const ScalarRef& ref)
{
  return test_isApprox(a+ref, b+ref);
}

template<typename Derived1, typename Derived2>
inline cond_t test_isMuchSmallerThan(const MatrixBase<Derived1>& m1,
                                   const MatrixBase<Derived2>& m2)
{
  return {m1.isMuchSmallerThan(m2, test_precision<typename internal::traits<Derived1>::Scalar>()), m1.absDistance(m2)};
}

template<typename Derived>
inline cond_t test_isMuchSmallerThan(const MatrixBase<Derived>& m,
                                   const typename NumTraits<typename internal::traits<Derived>::Scalar>::Real& s)
{
  return {m.isMuchSmallerThan(s, test_precision<typename internal::traits<Derived>::Scalar>()), s};
}

template<typename Derived>
inline cond_t test_isUnitary(const MatrixBase<Derived>& m)
{
  return {m.isUnitary(test_precision<typename internal::traits<Derived>::Scalar>()), 0.0};
}

template<typename T, typename U>
cond_t test_is_equal(const T& actual, const U& expected)
{
  return {actual == expected, std::abs(actual-expected)};
}

/** Creates a random Partial Isometry matrix of given rank.
  *
  * A partial isometry is a matrix all of whose singular values are either 0 or 1.
  * This is very useful to test rank-revealing algorithms.
  */
// Forward declaration to avoid ICC warning
template<typename MatrixType>
void createRandomPIMatrixOfRank(typename MatrixType::Index desired_rank, typename MatrixType::Index rows, typename MatrixType::Index cols, MatrixType& m);
template<typename MatrixType>
void createRandomPIMatrixOfRank(typename MatrixType::Index desired_rank, typename MatrixType::Index rows, typename MatrixType::Index cols, MatrixType& m)
{
  typedef typename internal::traits<MatrixType>::Index Index;
  typedef typename internal::traits<MatrixType>::Scalar Scalar;
  enum { Rows = MatrixType::RowsAtCompileTime, Cols = MatrixType::ColsAtCompileTime };

  typedef Matrix<Scalar, Dynamic, 1> VectorType;
  typedef Matrix<Scalar, Rows, Rows> MatrixAType;
  typedef Matrix<Scalar, Cols, Cols> MatrixBType;

  if(desired_rank == 0)
  {
    m.setZero(rows,cols);
    return;
  }

  if(desired_rank == 1)
  {
    // here we normalize the vectors to get a partial isometry
    m = VectorType::Random(rows).normalized() * VectorType::Random(cols).normalized().transpose();
    return;
  }

  MatrixAType a = MatrixAType::Random(rows,rows);
  MatrixType d = MatrixType::Identity(rows,cols);
  MatrixBType  b = MatrixBType::Random(cols,cols);

  // set the diagonal such that only desired_rank non-zero entries reamain
  const Index diag_size = (std::min)(d.rows(),d.cols());
  if(diag_size != desired_rank)
    d.diagonal().segment(desired_rank, diag_size-desired_rank) = VectorType::Zero(diag_size-desired_rank);

  HouseholderQR<MatrixAType> qra(a);
  HouseholderQR<MatrixBType> qrb(b);
  m = qra.householderQ() * d * qrb.householderQ();
}

// Forward declaration to avoid ICC warning
template<typename PermutationVectorType>
void randomPermutationVector(PermutationVectorType& v, typename PermutationVectorType::Index size);
template<typename PermutationVectorType>
void randomPermutationVector(PermutationVectorType& v, typename PermutationVectorType::Index size)
{
  typedef typename PermutationVectorType::Index Index;
  typedef typename PermutationVectorType::Scalar Scalar;
  v.resize(size);
  for(Index i = 0; i < size; ++i) v(i) = Scalar(i);
  if(size == 1) return;
  for(Index n = 0; n < 3 * size; ++n)
  {
    Index i = internal::random<Index>(0, size-1);
    Index j;
    do j = internal::random<Index>(0, size-1); while(j==i);
    std::swap(v(i), v(j));
  }
}

template<typename T> bool isNotNaN(const T& x)
{
  return x==x;
}

template<typename T> bool isNaN(const T& x)
{
  return x!=x;
}

template<typename T> bool isInf(const T& x)
{
  return x > NumTraits<T>::highest();
}

template<typename T> bool isMinusInf(const T& x)
{
  return x < NumTraits<T>::lowest();
}

} // end namespace Eigen

template<typename T> struct GetDifferentType;

template<> struct GetDifferentType<float> { typedef double type; };
template<> struct GetDifferentType<double> { typedef float type; };
template<typename T> struct GetDifferentType<std::complex<T> >
{ typedef std::complex<typename GetDifferentType<T>::type> type; };

// Forward declaration to avoid ICC warning
template<typename T> std::string type_name();
template<typename T> std::string type_name()              { return "other"; }
template<> std::string type_name<float>()                 { return "float"; }
template<> std::string type_name<double>()                { return "double"; }
template<> std::string type_name<int>()                   { return "int"; }
template<> std::string type_name<std::complex<float> >()  { return "complex<float>"; }
template<> std::string type_name<std::complex<double> >() { return "complex<double>"; }
template<> std::string type_name<std::complex<int> >()    { return "complex<int>"; }

// forward declaration of the main test function
void EIGEN_CAT(test_,EIGEN_TEST_FUNC)();

using namespace Eigen;

inline void set_repeat_from_string(const char *str)
{
  errno = 0;
  g_repeat = int(strtoul(str, 0, 10));
  if(errno || g_repeat <= 0)
  {
    std::cout << "Invalid repeat value " << str << std::endl;
    exit(EXIT_FAILURE);
  }
  g_has_set_repeat = true;
}

inline void set_seed_from_string(const char *str)
{
  errno = 0;
  g_seed = int(strtoul(str, 0, 10));
  if(errno || g_seed == 0)
  {
    std::cout << "Invalid seed value " << str << std::endl;
    exit(EXIT_FAILURE);
  }
  g_has_set_seed = true;
}

// These warning are disabled here such that they are still ON when parsing Eigen's header files.
#if defined __INTEL_COMPILER
  // remark #383: value copied to temporary, reference to temporary used
  //  -> this warning is raised even for legal usage as: g_test_stack.push_back("foo"); where g_test_stack is a std::vector<std::string>
  // remark #1418: external function definition with no prior declaration
  //  -> this warning is raised for all our test functions. Declaring them static would fix the issue.
  // warning #279: controlling expression is constant
  // remark #1572: floating-point equality and inequality comparisons are unreliable
  #pragma warning disable 279 383 1418 1572
#endif
