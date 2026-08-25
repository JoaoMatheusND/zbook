/*******************************************************************
 * @file zbook_i2c.c
 *
 * @brief Implementation of the Zbook I2C protocol interface.
 * @author Matheus Macário dos Santos (matheus.macario@edge.ufal.br)
 * @version 0.1
 * @date 24/08/2026
 *
 * @copyright Copyright (c) 2026
 *
 *******************************************************************/

#include <protocols/zbook_i2c.h>

#include <errno.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>

static const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

int zbook_i2c_init(void)
{
  if (!device_is_ready(i2c_dev)) {
    return -ENODEV;
  }

  return 0;
}

int zbook_i2c_write(uint16_t addr, const uint8_t *data, size_t len)
{
  if (!device_is_ready(i2c_dev)) {
    return -ENODEV;
  }

  return i2c_write(i2c_dev, data, len, addr);
}

int zbook_i2c_read(uint16_t addr, uint8_t *data, size_t len)
{
  if (!device_is_ready(i2c_dev)) {
    return -ENODEV;
  }

  return i2c_read(i2c_dev, data, len, addr);
}

int zbook_i2c_write_read(uint16_t addr, const uint8_t *write_data, size_t write_len,
                          uint8_t *read_data, size_t read_len)
{
  if (!device_is_ready(i2c_dev)) {
    return -ENODEV;
  }

  return i2c_write_read(i2c_dev, addr, write_data, write_len, read_data, read_len);
}

int zbook_i2c_cfg(enum zbook_i2c_speed speed)
{
  if (!device_is_ready(i2c_dev)) {
    return -ENODEV;
  }

  uint32_t dev_config = I2C_MODE_CONTROLLER | I2C_SPEED_SET((uint32_t)speed);

  return i2c_configure(i2c_dev, dev_config);
}
