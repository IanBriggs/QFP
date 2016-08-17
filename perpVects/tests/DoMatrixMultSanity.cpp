#include "testBase.h"
#include "QFPHelpers.h"

#include <cmath>
#include <typeinfo>

template <typename T>
class DoMatrixMultSanity: public QFPTest::TestBase {
public:
  DoMatrixMultSanity(std::string id) : QFPTest::TestBase(id){}

  QFPTest::resultType operator()(const QFPTest::testInput& ti) {
    auto dim = ti.highestDim;
    T min = ti.min;
    T max = ti.max;
    QFPHelpers::Vector<T> b = QFPHelpers::Vector<T>::getRandomVector(dim, min, max);
    auto c = QFPHelpers::Matrix<T>::Identity(dim) * b;
    QFPHelpers::info_stream << "Product is: " << c << std::endl;
    bool eq = c == b;
    QFPHelpers::info_stream << "A * b == b? " << eq << std::endl;
    return {{
      {id, typeid(T).name()}, {c.L1Distance(b), c.LInfDistance(b)}
    }};
  }
};

REGISTER_TYPE(DoMatrixMultSanity)