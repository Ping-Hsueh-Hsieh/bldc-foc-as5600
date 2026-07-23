#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include "i2c.h"
#include "rte.h"
#include "stm32h7xx_hal.h"
#include "util.h"
#include "as5600.h"

#define AS5600_I2C_DEV_ADDR_SHIFT (AS5600_I2C_ADDR << 1)
#define AS5600_I2C_REG_RAW_ANGLE (0x0C)

static volatile bool i2c_mem_read_done = true;
static uint8_t buf[2] = {0};
static float update_period_s = 1e-3;

void AS5600_init(void)
{
  AS5600_read_angle();
  while (!i2c_mem_read_done){};
  AS5600_angle.ang_rad_prev = AS5600_angle.ang_rad;
}

void AS5600_read_angle(void)
{
  if (!i2c_mem_read_done) {
    UNREACHABLE();
    return;
  }
  i2c_mem_read_done = false;
  if (HAL_I2C_Mem_Read_DMA(&hi2c1, AS5600_I2C_DEV_ADDR_SHIFT, AS5600_I2C_REG_RAW_ANGLE, I2C_MEMADD_SIZE_8BIT, buf, 2) != HAL_OK) {
    UNREACHABLE();
  }
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef* hi2c)
{
  (void)hi2c;
  __NOP();
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef* hi2c)
{
  if (hi2c->Instance == hi2c1.Instance) {
    i2c_mem_read_done = true;
    uint16_t raw = ((uint16_t)buf[0] << 8) | buf[1];
    raw &= 0x0FFF;
    float ratio = ((float)raw + 1.0f) / 4096.0f;
    float deg = ratio * 360.0f;
    float rad = ratio * 2 * M_PI;
    AS5600_angle.raw = raw;
    AS5600_angle.ang_deg = deg;
    AS5600_angle.ang_vel_rad_s = (rad - AS5600_angle.ang_rad) / update_period_s;
    AS5600_angle.ang_rad_prev = AS5600_angle.ang_rad;
    AS5600_angle.ang_rad = rad;
  }
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef* hi2c)
{
  (void)hi2c;
  __NOP();
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef* hi2c)
{
  (void)hi2c;
  __NOP();
}
