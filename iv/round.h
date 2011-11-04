#ifndef IV_ROUND_H_
#define IV_ROUND_H_
#include <cmath>
namespace iv {
namespace core {

inline double Round(const double& value) {
  const double res = std::ceil(value);
  if (res - value > 0.5) {
    return res - 1;
  } else {
    return res;
  }
}

} }  // namespace iv::core
#endif  // IV_ROUND_H_