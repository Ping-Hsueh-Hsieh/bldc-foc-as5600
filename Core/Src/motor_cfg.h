#ifndef MOTOR_CFG_H_
#define MOTOR_CFG_H_

#define CCR_A (TIM1->CCR2)
#define CCR_B (TIM4->CCR4)
#define CCR_C (TIM1->CCR1)

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
