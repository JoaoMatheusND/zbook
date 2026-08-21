/*******************************************************************
 * @file zbook_uart.h
 *
 * @brief Interface file for the Zbook UART protocol.
 * @author João Matheus Nascimento Dias (joao.dias@edge.ufal.br)
 * @version 0.1
 * @date 21/08/2026
 *
 * @copyright Copyright (c) 2026
 *
 *******************************************************************/

#ifndef _ZBOOK_UART_H
#define _ZBOOK_UART_H

#include <stddef.h>
#include <stdint.h>

#include <zephyr/drivers/uart.h>

/**
 * @brief Parity mode for the zbook UART peripheral.
 */
enum zbook_uart_parity {
	ZBOOK_UART_PARITY_NONE = UART_CFG_PARITY_NONE, /**< Uart none parity */
	ZBOOK_UART_PARITY_ODD = UART_CFG_PARITY_ODD,   /**< Uart odd parity */
	ZBOOK_UART_PARITY_EVEN = UART_CFG_PARITY_EVEN, /**< Uart even parity */
};

/**
 * @brief Number of stop bits for the zbook UART peripheral.
 */
enum zbook_uart_stop_bits {
	ZBOOK_UART_STOP_BITS_1 = UART_CFG_STOP_BITS_1, /**< Uart 1 stop bit */
	ZBOOK_UART_STOP_BITS_2 = UART_CFG_STOP_BITS_2, /**< Uart 2 stop bits */
};

/**
 * @brief Number of data bits for the zbook UART peripheral.
 */
enum zbook_uart_data_bits {
	ZBOOK_UART_DATA_BITS_7 = UART_CFG_DATA_BITS_7, /**< Uart 7 data bits */
	ZBOOK_UART_DATA_BITS_8 = UART_CFG_DATA_BITS_8, /**< Uart 8 data bits */
	ZBOOK_UART_DATA_BITS_9 = UART_CFG_DATA_BITS_9, /**< Uart 9 data bits */
};

/**
 * @brief Runtime configuration for the zbook UART peripheral.
 */
struct zbook_uart_cfg {
	uint32_t baudrate;                   /**< Baudrate for the UART */
	enum zbook_uart_parity parity;       /**< Parity for the UART */
	enum zbook_uart_stop_bits stop_bits; /**< Stop bits for the UART */
	enum zbook_uart_data_bits data_bits; /**< Data bits for the UART */
};

/**
 * @brief Callback signature invoked when the zbook UART peripheral has
 * data available to read. Called from ISR context.
 *
 * @param dev UART device that triggered the callback.
 * @param user_data User data passed at registration.
 */
typedef void (*zbook_uart_cb_t)(const struct device *dev, void *user_data);

/**
 * @brief Initializes the zbook UART peripheral.
 *
 * @return 0 on success, -errno on error.
 */
int zbook_uart_init(void);

/**
 * @brief Writes data to the zbook UART peripheral.
 *
 * @param data Buffer to write.
 * @param len Number of bytes to write.
 *
 * @return 0 on success, -errno on error.
 */
int zbook_uart_write(const uint8_t *data, size_t len);

/**
 * @brief Reads data from the zbook UART peripheral.
 *
 * @param data Buffer to store the read bytes.
 * @param len Number of bytes to read.
 *
 * @return 0 on success, -errno on error.
 */
int zbook_uart_read(uint8_t *data, size_t len);

/**
 * @brief Reconfigures the zbook UART peripheral at runtime.
 *
 * @param cfg Desired UART configuration.
 *
 * @return 0 on success, -errno on error.
 */
int zbook_uart_cfg(const struct zbook_uart_cfg *cfg);

/**
 * @brief Registers a callback for RX-ready events on the zbook UART
 * peripheral and enables RX interrupts.
 *
 * @param cb Callback invoked from ISR context when RX data is ready.
 * @param user_data Opaque pointer passed back to the callback.
 *
 * @return 0 on success, -errno on error.
 */
int zbook_uart_set_callback(zbook_uart_cb_t cb, void *user_data);

#endif /* _ZBOOK_UART_H */
