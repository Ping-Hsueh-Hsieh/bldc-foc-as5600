// #include "lowpass_filter.h"
#include "motor_cfg.h"
#include "pid.h"
#include "rte.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_gpio.h"
#include "tim.h"
#include "util.h"

#define _SQRT3 1.73205080757f
#define _SQRT3_2 0.86602540378f
#define _1_SQRT3 0.57735026919f
#define _2_SQRT3 1.15470053838f

static float _normalizeAngle(float angle);
static float _electricalAngle(float shaft_angle, int pole_pairs);
static void setPwm(float Ua, float Ub, float Uc, TIM_TypeDef* TIM_BASE);
static void setPhaseVoltage(float Uq, float Ud, float angle_el, TIM_TypeDef* TIM_BASE);

// static float cal_angular_vel(float angle_now);

// static struct LowPassFilter filter = {.Tf = 0.01, .y_prev = 0.0f};  //Tf=10ms
// static struct PIDController pid_controller = {.P = 0.5, .I = 0.1, .D = 0.0, .output_ramp = 100.0, .limit = 6, .error_prev = 0, .output_prev = 0, .integral_prev = 0};

typedef struct
{
  const MotorParam* motor_param;
  float init_ang_rad;
  float target_vel_rad_p_s;
  float target_ang_rad;
} State;

static float Kp = 0.167f;
static float voltage_power_supply = 12;
int period = 0;  // period for the PWM

static State g_state = {0};

void motor_init(void)
{
  period = htim1.Init.Period + 1;
  g_state.motor_param = &MotorParam_2804;
  g_state.target_vel_rad_p_s = 5;
  g_state.target_ang_rad = M_PI / 3.f;

  g_state.init_ang_rad = _electricalAngle(AS5600_angle.ang_rad, g_state.motor_param->pole_pairs);
  HAL_TIM_Base_Start_IT(&htim2);

  // enable the drive board
  HAL_GPIO_WritePin(MOTOR_EN_GPIO_Port, MOTOR_EN_Pin, GPIO_PIN_SET);
}

static void svpwm(void)
{
#if 0
    float ang_rad = AS5600_angle.ang_rad;
    float angular_vel = cal_angular_vel(ang_rad);
    float filtered_vel = LowPassFilter_operator(angular_vel, &filter);
    float velo_controller_out = PID_operator(g_state.target_vel - filtered_vel, &pid_controller);
    read_ADC_voltage(hadc1, ADC_VAL);
    for (int i = 0; i < 3; i++) {
      current_phase[i] = (float)(ADC_VAL[i] - current_offset[i]) / (float)adc_range * Vref / Rsense;
    }

    float Iq = cal_Iq(current_phase, _electricalAngle(angle_now, pole_pairs));
    float filtered_Iq = LowPassFilter_operator(Iq, &filter_current);

    float current_controller_output = PID_operator(velo_controller_out - filtered_Iq, &pid_controller_current);

    setPhaseVoltage(_constrain(current_controller_output, -voltage_power_supply / 2, voltage_power_supply / 2), 0, _electricalAngle(angle_now, pole_pairs), TIM1);
#endif
}

static float Uq = 0.1;

static void ang_ctrl(void)
{
  float ang_rad = AS5600_angle.ang_rad;
  // float angle_error = g_state.target_ang_rad - ang_rad;
  // angle_error = _normalizeAngle(angle_error);
  // if (angle_error > M_PI) {
  //   angle_error -= 2 * M_PI;
  // }
  // float Uq = _constrain(Kp * (angle_error) / M_PI * 180, -voltage_power_supply / 2, voltage_power_supply / 2);
  setPhaseVoltage(Uq, 0, _electricalAngle(ang_rad, g_state.motor_param->pole_pairs), TIM1);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
  if (htim->Instance == TIM2) {
    ang_ctrl();
  }
}

// static float cal_angular_vel(float angle_now)
// {
//   if (angle_prev < 0) {
//     angle_prev = angle_now;
//     return 0;
//   }
//   float delta_angle = angle_now - angle_prev;
//   if (delta_angle >= 1.6 * M_PI) {
//     delta_angle -= 2 * M_PI;
//   }
//   if (delta_angle <= -1.6 * M_PI) {
//     delta_angle += 2 * M_PI;
//   }
//   angle_prev = angle_now;
//   return delta_angle / Ts;
// }

float _normalizeAngle(float angle)
{
  float a = fmod(angle, 2 * M_PI);  //取余运算可以用于归一化，列出特殊值例子算便知
  return a >= 0 ? a : (a + 2 * M_PI);
  //三目运算符。格式：condition ? expr1 : expr2
  //其中，condition 是要求值的条件表达式，如果条件成立，则返回 expr1 的值，否则返回 expr2 的值。
  //可以将三目运算符视为 if-else 语句的简化形式。
  //fmod 函数的余数的符号与除数相同。因此，当 angle 的值为负数时，余数的符号将与 _2M_PI 的符号相反。
  //也就是说，如果 angle 的值小于 0 且 _2M_PI 的值为正数，则 fmod(angle, _2M_PI) 的余数将为负数。
  //例如，当 angle 的值为 -M_PI/2，_2M_PI 的值为 2M_PI 时，fmod(angle, _2M_PI) 将返回一个负数。
  //在这种情况下，可以通过将负数的余数加上 _2M_PI 来将角度归一化到 [0, 2M_PI] 的范围内，以确保角度的值始终为正数。
}

float _electricalAngle(float shaft_angle, int pole_pairs)
{
  return _normalizeAngle(((float)pole_pairs * shaft_angle) - g_state.init_ang_rad);
}

float g_dc_a = 0;
float g_dc_b = 0;
float g_dc_c = 0;

void setPwm(float Ua, float Ub, float Uc, TIM_TypeDef* TIM_BASE)
{

  // 限制上限
  Ua = _constrain(Ua, 0.0f, g_state.motor_param->limit_volt);
  Ub = _constrain(Ub, 0.0f, g_state.motor_param->limit_volt);
  Uc = _constrain(Uc, 0.0f, g_state.motor_param->limit_volt);
  // 计算占空比
  // 限制占空比从0到1
  float dc_a = _constrain(Ua / voltage_power_supply, 0.0f, 1.0f);
  float dc_b = _constrain(Ub / voltage_power_supply, 0.0f, 1.0f);
  float dc_c = _constrain(Uc / voltage_power_supply, 0.0f, 1.0f);

  g_dc_a = dc_a;
  g_dc_b = dc_b;
  g_dc_c = dc_c;

  //写入PWM到PWM 0 1 2 通道
  TIM_BASE->CCR1 = (uint32_t)roundf(dc_a * period);
  TIM_BASE->CCR2 = (uint32_t)roundf(dc_b * period);
  TIM_BASE->CCR3 = (uint32_t)roundf(dc_c * period);
}

void setPhaseVoltage(float Uq, float Ud, float angle_el, TIM_TypeDef* TIM_BASE)
{
  (void)Ud;
  angle_el = _normalizeAngle(angle_el + g_state.init_ang_rad);
  // 帕克逆变换
  float Ualpha = -Uq * sin(angle_el);
  float Ubeta = Uq * cos(angle_el);

  // 克拉克逆变换
  float Ua = Ualpha + voltage_power_supply / 2;
  float Ub = (sqrt(3) * Ubeta - Ualpha) / 2 + voltage_power_supply / 2;
  float Uc = (-Ualpha - sqrt(3) * Ubeta) / 2 + voltage_power_supply / 2;
  setPwm(Ua, Ub, Uc, TIM_BASE);
}
