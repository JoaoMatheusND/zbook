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

#include <protocols/zbook_uart.h>

#include <errno.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

static const struct device *uart_dev = DEVICE_DT_GET(DT_ALIAS(zbook_uart));

static zbook_uart_cb_t user_cb;
static void *user_cb_data;

static void zbook_uart_isr(const struct device *dev, void *data)
{
  if (uart_irq_update(dev) <= 0 || !uart_irq_is_pending(dev)) {
    return;
  }

  if (uart_irq_rx_ready(dev) && user_cb) {
    user_cb(dev, data);
  }
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

int zbook_uart_set_callback(zbook_uart_cb_t cb, void *user_data)
{
  if (!device_is_ready(uart_dev)) {
    return -ENODEV;
  }

  user_cb = cb;
  user_cb_data = user_data;

  int err = uart_irq_callback_user_data_set(uart_dev, zbook_uart_isr, user_cb_data);
  if (err) {
    return err;
  }

  uart_irq_rx_enable(uart_dev);

  return 0;
}
