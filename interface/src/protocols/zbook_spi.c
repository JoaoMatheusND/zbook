/*******************************************************************
 * @file zbook_spi.c
 *
 * @brief Implements the interface for the zbook SPI bus.
 * @author José Félix de Oliveira Neto (josefelix.neto@edge.ufal.br)
 * @version 0.1
 * @date 25/08/26
 *
 * @copyright Copyright (c) 2026
 *
 *******************************************************************/

#include "protocols/zbook_spi.h"

#ifdef CONFIG_ZBOOK_SPI

#include <errno.h>

#include <zephyr/devicetree.h>
#include <zephyr/drivers/spi.h>

#define ZBOOK_SPI_NODE DT_NODELABEL(zbook_spi)
#define ZBOOK_SPI_OP   (SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_OP_MODE_MASTER)

static struct spi_dt_spec zbook_spi_dev = SPI_DT_SPEC_GET(ZBOOK_SPI_NODE, ZBOOK_SPI_OP);

int zbook_spi_init(void)
{
	if (!spi_is_ready_dt(&zbook_spi_dev)) {
		return -ENODEV;
	}

	return 0;
}

int zbook_spi_configure(const struct zbook_spi_cfg *cfg)
{
	if (cfg == NULL || cfg->word_size == 0 || cfg->word_size > 32) {
		return -EINVAL;
	}

	uint32_t operation = SPI_WORD_SET(cfg->word_size) | SPI_OP_MODE_MASTER;

	switch (cfg->mode) {
	case ZBOOK_SPI_MODE_0:
		break;
	case ZBOOK_SPI_MODE_1:
		operation |= SPI_MODE_CPHA;
		break;
	case ZBOOK_SPI_MODE_2:
		operation |= SPI_MODE_CPOL;
		break;
	case ZBOOK_SPI_MODE_3:
		operation |= SPI_MODE_CPOL | SPI_MODE_CPHA;
		break;
	default:
		return -EINVAL;
	}

	if (cfg->bit_order == ZBOOK_SPI_LSB_FIRST) {
		operation |= SPI_TRANSFER_LSB;
	}

	zbook_spi_dev.config.frequency = cfg->frequency;
	zbook_spi_dev.config.operation = operation;

	return 0;
}

int zbook_spi_transceive(const uint8_t *tx, uint8_t *rx, size_t len)
{
	if ((tx == NULL && rx == NULL) || len == 0) {
		return -EINVAL;
	}

	const struct spi_buf tx_buf = {
		.buf = (void *)tx,
		.len = len,
	};
	const struct spi_buf_set tx_bufs = {
		.buffers = &tx_buf,
		.count = 1,
	};

	struct spi_buf rx_buf = {
		.buf = rx,
		.len = len,
	};
	const struct spi_buf_set rx_bufs = {
		.buffers = &rx_buf,
		.count = 1,
	};

	return spi_transceive_dt(&zbook_spi_dev, tx != NULL ? &tx_bufs : NULL,
				 rx != NULL ? &rx_bufs : NULL);
}

int zbook_spi_write(const uint8_t *data, size_t len)
{
	if (data == NULL || len == 0) {
		return -EINVAL;
	}

	return zbook_spi_transceive(data, NULL, len);
}

int zbook_spi_read(uint8_t *data, size_t len)
{
	if (data == NULL || len == 0) {
		return -EINVAL;
	}

	return zbook_spi_transceive(NULL, data, len);
}

#endif /* CONFIG_ZBOOK_SPI */
