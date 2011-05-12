#ifndef _IV_CAS_WIN_H_
#define _IV_CAS_WIN_H_
#include "windows.h"
namespace iv {
namespace core {
namespace thread {

inline int CompareAndSwap(volatile LONG* target,
                          LONG new_value, LONG old_value) {
  return InterlockedCompareExchange(target, new_value, old_value);
}

} } }  // iv::core::thread
#endif  // _IV_CAS_WIN_H_