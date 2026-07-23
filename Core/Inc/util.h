#ifndef UTIL_H_
#define UTIL_H_

#define DEV_ASSERT(cond, ...) \
  do {                        \
    while (!cond) {}          \
  } while (0)
#define UNREACHABLE(...) DEV_ASSERT(0, __VA_ARGS__)

#define _constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

#endif  // UTIL_H_
