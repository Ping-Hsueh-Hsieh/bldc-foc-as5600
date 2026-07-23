#include "motor_cfg.h"

MotorParam MotorParam_2804 = {
  .pole_pairs = 7.0f,
  .rated_rpm = 2600.0f,
  .flux = 0.0035f,
  .r =
    {
      .phase = 2.3,
      .winding = 5.1,
      .d = 2.3,
      .q = 2.3,
    },
  .l =
    {
      .phase = 0.86e-3,
      .winding = 2.8e-3,
      .d = 0.86e-3,
      .q = 0.86e-3,
    },
  .limit_volt = 12.f,
};
