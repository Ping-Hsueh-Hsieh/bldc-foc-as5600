#ifndef MOTOR_CFG_H_
#define MOTOR_CFG_H_

typedef struct {
    float pole_pairs;
    float rated_rpm;
    float flux;
    struct {
        float phase;
        float winding;
        float d;
        float q;
    } r;
    struct {
        float phase;
        float winding;
        float d;
        float q;
    } l;
    float limit_volt;
} MotorParam;

extern MotorParam MotorParam_2804;

#endif  // MOTOR_CFG_H_
