/*******************************************************************
 * @file zbook_i2c.h
 *
 * @brief Interface file for the Zbook I2C protocol.
 * @author Matheus Macário dos Santos (matheus.macario@edge.ufal.br)
 * @version 0.1
 * @date 24/08/2026
 *
 * @copyright Copyright (c) 2026
 *
 *******************************************************************/

#ifndef _ZBOOK_I2C_H
#define _ZBOOK_I2C_H

#include <stddef.h>
#include <stdint.h>

#include <zephyr/drivers/i2c.h>

/**
 * @brief Bus speed for the zbook I2C peripheral.
 */
enum zbook_i2c_speed {
	ZBOOK_I2C_SPEED_STANDARD = I2C_SPEED_STANDARD,   /**< 100 kHz */
	ZBOOK_I2C_SPEED_FAST = I2C_SPEED_FAST,           /**< 400 kHz */
	ZBOOK_I2C_SPEED_FAST_PLUS = I2C_SPEED_FAST_PLUS, /**< 1 MHz */
};

/**
 * @brief Initializes the zbook I2C peripheral.
 *
 * @return 0 on success, -errno on error.
 */
int zbook_i2c_init(void);

/**
 * @brief Writes data to an I2C device.
 *
 * @param addr 7-bit address of the target I2C device.
 * @param data Buffer to write.
 * @param len Number of bytes to write.
 *
 * @return 0 on success, -errno on error.
 */
int zbook_i2c_write(uint16_t addr, const uint8_t *data, size_t len);

/**
 * @brief Reads data from an I2C device.
 *
 * @param addr 7-bit address of the target I2C device.
 * @param data Buffer to store the read bytes.
 * @param len Number of bytes to read.
 *
 * @return 0 on success, -errno on error.
 */
int zbook_i2c_read(uint16_t addr, uint8_t *data, size_t len);

/**
 * @brief Writes then reads data from an I2C device in a single transaction.
 *
 * @param addr Address of the target I2C device.
 * @param write_data Pointer to the data to be written.
 * @param write_len Number of bytes to write.
 * @param read_data Pointer to storage for the read data.
 * @param read_len Number of bytes to read.
 *
 * @return 0 on success, -errno on error.
 */
int zbook_i2c_write_read(uint16_t addr, const uint8_t *write_data, size_t write_len,
			  uint8_t *read_data, size_t read_len);

/**
 * @brief Reconfigures the zbook I2C peripheral bus speed at runtime.
 *
 * @param speed Desired I2C bus speed.
 *
 * @return 0 on success, -errno on error.
 */
int zbook_i2c_cfg(enum zbook_i2c_speed speed);

#endif /* _ZBOOK_I2C_H */
