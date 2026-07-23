#ifndef RTE_H_
#define RTE_H_

#include <stdint.h>

typedef struct {
    uint16_t raw;
    float ang_deg;
    float ang_rad;
    float ang_rad_prev;
    float ang_vel_rad_s;
} AS5600_Angle_Type;

extern AS5600_Angle_Type AS5600_angle;

#endif  // RTE_H_
