/**
 * @file zbook_spi.h
 * @brief Pure C passthrough interface for the zbook generic SPI bus.
 * @author José Félix de O. Neto <josefelix.neto@edge.ufal.br>
 *
 */

#ifndef ZBOOK_SPI_H
#define ZBOOK_SPI_H

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Check that the zbook SPI bus is ready to use.
 *
 * @return 0 on success, -errno on error.
 */
int zbook_spi_init(void);

/**
 * @brief Exchange @p len bytes over the zbook SPI bus in a single transaction
 * (one CS assertion).
 *
 * At least one of @p tx / @p rx must be non-NULL. Passing NULL for @p tx
 * sends dummy bytes; passing NULL for @p rx discards the received bytes.
 *
 * @param tx  Buffer to send, or NULL to send dummy bytes.
 * @param rx  Buffer to receive into, or NULL to discard received bytes.
 * @param len Number of bytes to exchange.
 * @return 0 on success, -errno on error.
 */
int zbook_spi_transceive(const uint8_t *tx, uint8_t *rx, size_t len);

/**
 * @brief Write @p len bytes from @p data over the zbook SPI bus.
 *
 * @param data Buffer to send.
 * @param len  Number of bytes to send.
 * @return 0 on success, -errno on error.
 */
int zbook_spi_write(const uint8_t *data, size_t len);

/**
 * @brief Read @p len bytes into @p data from the zbook SPI bus.
 *
 * @param data Buffer to receive into.
 * @param len  Number of bytes to receive.
 * @return 0 on success, -errno on error.
 */
int zbook_spi_read(uint8_t *data, size_t len);

#endif /* ZBOOK_SPI_H */
