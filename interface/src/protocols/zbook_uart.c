/*******************************************************************
 * @file zbook_uart.c
 *
 * @brief Implementation of the Zbook UART protocol interface.
 * @author João Matheus Nascimento Dias (joao.dias@edge.ufal.br)
 * @version 0.1
 * @date 21/08/2026
 *
 * @copyright Copyright (c) 2026
 *
 *******************************************************************/

#include "protocols/zbook_uart.h"

#include <errno.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/misc/pio_rpi_pico/pio_rpi_pico.h>

#include <hardware/clocks.h>
#include <hardware/pio.h>

static const struct device *uart_dev = DEVICE_DT_GET(DT_ALIAS(zbook_uart));

/* Cycles the uart_tx/uart_rx PIO programs below take per bit. */
#define CYCLES_PER_BIT 8
#define SIDESET_BIT_COUNT 2

/* Same PIO programs used by the raspberrypi,pico-uart-pio Zephyr driver,
 * reused here so a pin can be registered as UART at runtime instead of
 * through a devicetree instance. */
RPI_PICO_PIO_DEFINE_PROGRAM(uart_tx, 0, 3,
		/* .wrap_target */
	0x9fa0, /*  0: pull   block           side 1 [7]  */
	0xf727, /*  1: set    x, 7            side 0 [7]  */
	0x6001, /*  2: out    pins, 1                     */
	0x0642, /*  3: jmp    x--, 2                 [6]  */
		/* .wrap */
);

RPI_PICO_PIO_DEFINE_PROGRAM(uart_rx, 1, 8,
	0x20a0, /*  0: wait   1 pin, 0                    */
		/*  .wrap_target */
	0x2020, /*  1: wait   0 pin, 0                    */
	0xea27, /*  2: set    x, 7                   [10] */
	0x4001, /*  3: in     pins, 1                     */
	0x0643, /*  4: jmp    x--, 3                 [6]  */
	0x00c8, /*  5: jmp    pin, 8                      */
	0xc014, /*  6: irq    nowait 4 rel                */
	0x0000, /*  7: jmp    0                           */
	0x8020, /*  8: push   block                       */
		/*  .wrap */
);

static bool pio_uart_active;
static PIO pio_uart_pio;
static size_t pio_uart_tx_sm;
static size_t pio_uart_rx_sm;

static int pio_uart_tx_init(PIO pio, size_t sm, uint32_t tx_pin, float div)
{
  if (!pio_can_add_program(pio, RPI_PICO_PIO_GET_PROGRAM(uart_tx))) {
    return -EBUSY;
  }

  uint32_t offset = pio_add_program(pio, RPI_PICO_PIO_GET_PROGRAM(uart_tx));
  pio_sm_config sm_config = pio_get_default_sm_config();

  sm_config_set_sideset(&sm_config, SIDESET_BIT_COUNT, true, false);
  sm_config_set_out_shift(&sm_config, true, false, 0);
  sm_config_set_out_pins(&sm_config, tx_pin, 1);
  sm_config_set_sideset_pins(&sm_config, tx_pin);
  sm_config_set_fifo_join(&sm_config, PIO_FIFO_JOIN_TX);
  sm_config_set_clkdiv(&sm_config, div);
  sm_config_set_wrap(&sm_config, offset + RPI_PICO_PIO_GET_WRAP_TARGET(uart_tx),
                      offset + RPI_PICO_PIO_GET_WRAP(uart_tx));

  pio_gpio_init(pio, tx_pin);
  pio_sm_set_pins_with_mask(pio, sm, BIT(tx_pin), BIT(tx_pin));
  pio_sm_set_pindirs_with_mask(pio, sm, BIT(tx_pin), BIT(tx_pin));
  pio_sm_init(pio, sm, offset, &sm_config);
  pio_sm_set_enabled(pio, sm, true);

  return 0;
}

static int pio_uart_rx_init(PIO pio, size_t sm, uint32_t rx_pin, float div)
{
  if (!pio_can_add_program(pio, RPI_PICO_PIO_GET_PROGRAM(uart_rx))) {
    return -EBUSY;
  }

  uint32_t offset = pio_add_program(pio, RPI_PICO_PIO_GET_PROGRAM(uart_rx));
  pio_sm_config sm_config = pio_get_default_sm_config();

  pio_gpio_init(pio, rx_pin);
  pio_sm_set_consecutive_pindirs(pio, sm, rx_pin, 1, false);
  sm_config_set_in_pins(&sm_config, rx_pin);
  sm_config_set_jmp_pin(&sm_config, rx_pin);
  sm_config_set_in_shift(&sm_config, true, false, 0);
  sm_config_set_fifo_join(&sm_config, PIO_FIFO_JOIN_RX);
  sm_config_set_clkdiv(&sm_config, div);
  sm_config_set_wrap(&sm_config, offset + RPI_PICO_PIO_GET_WRAP_TARGET(uart_rx),
                      offset + RPI_PICO_PIO_GET_WRAP(uart_rx));

  pio_sm_init(pio, sm, offset, &sm_config);
  pio_sm_set_enabled(pio, sm, true);

  return 0;
}

int zbook_uart_register_pin(uint32_t tx_pin, uint32_t rx_pin, uint32_t baudrate)
{
  if (tx_pin >= ZBOOK_UART_PIO_GPIO_COUNT || rx_pin >= ZBOOK_UART_PIO_GPIO_COUNT) {
    return -EINVAL;
  }

  if (tx_pin == rx_pin) {
    return -EINVAL;
  }

  if (baudrate == 0) {
    return -EINVAL;
  }

  const struct device *pio_dev = DEVICE_DT_GET(DT_ALIAS(zbook_pio));

  if (!device_is_ready(pio_dev)) {
    return -ENODEV;
  }

  PIO pio = pio_rpi_pico_get_pio(pio_dev);
  float div = (float)clock_get_hz(clk_sys) / (CYCLES_PER_BIT * baudrate);
  size_t tx_sm;
  size_t rx_sm;
  int err;

  err = pio_rpi_pico_allocate_sm(pio_dev, &tx_sm);
  if (err) {
    return err;
  }

  err = pio_rpi_pico_allocate_sm(pio_dev, &rx_sm);
  if (err) {
    return err;
  }

  err = pio_uart_tx_init(pio, tx_sm, tx_pin, div);
  if (err) {
    return err;
  }

  err = pio_uart_rx_init(pio, rx_sm, rx_pin, div);
  if (err) {
    return err;
  }

  pio_uart_pio = pio;
  pio_uart_tx_sm = tx_sm;
  pio_uart_rx_sm = rx_sm;
  pio_uart_active = true;

  return 0;
}

static void pio_uart_write_byte(uint8_t c)
{
  pio_sm_put_blocking(pio_uart_pio, pio_uart_tx_sm, (uint32_t)c);
}

static int pio_uart_read_byte(uint8_t *c)
{
  /* The RX FIFO is 4 bytes wide; the received byte lands in the MSB. */
  io_rw_8 *rx_fifo_msb = (io_rw_8 *)&pio_uart_pio->rxf[pio_uart_rx_sm] + 3;

  if (pio_sm_is_rx_fifo_empty(pio_uart_pio, pio_uart_rx_sm)) {
    return -EAGAIN;
  }

  *c = (uint8_t)*rx_fifo_msb;
  return 0;
}

int zbook_uart_init(void)
{
  if (!device_is_ready(uart_dev)) {
    return -ENODEV;
  }

  return 0;
}

int zbook_uart_write(const uint8_t *data, size_t len)
{
  if (pio_uart_active) {
    for (size_t i = 0; i < len; i++) {
      pio_uart_write_byte(data[i]);
    }

    return 0;
  }

  if (!device_is_ready(uart_dev)) {
    return -ENODEV;
  }

  for (size_t i = 0; i < len; i++) {
    uart_poll_out(uart_dev, data[i]);
  }

  return 0;
}

int zbook_uart_read(uint8_t *data, size_t len)
{
  if (pio_uart_active) {
    for (size_t i = 0; i < len; i++) {
      int err = pio_uart_read_byte(&data[i]);
      if (err) {
        return err;
      }
    }

    return 0;
  }

  if (!device_is_ready(uart_dev)) {
    return -ENODEV;
  }

  for (size_t i = 0; i < len; i++) {
    int err = uart_poll_in(uart_dev, &data[i]);
    if (err) {
      return err;
    }
  }

  return 0;
}

int zbook_uart_cfg(const struct zbook_uart_cfg *cfg)
{
  if (cfg->baudrate == 0) {
    return -EINVAL;
  }

  if (!device_is_ready(uart_dev)) {
    return -ENODEV;
  }

  struct uart_config uart_cfg = {
    .baudrate = cfg->baudrate,
    .parity = (uint8_t)cfg->parity,
    .stop_bits = (uint8_t)cfg->stop_bits,
    .data_bits = (uint8_t)cfg->data_bits,
    .flow_ctrl = UART_CFG_FLOW_CTRL_NONE,
  };

  return uart_configure(uart_dev, &uart_cfg);
}
