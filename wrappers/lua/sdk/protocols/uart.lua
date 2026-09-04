---@meta

--- ZBook generic UART protocol binding (zbook.protocols.uart).
--- Wraps zbook_uart_init/write/read/cfg/register_pin from
--- interface/includes/protocols/zbook_uart.h.

zbook = zbook or {}
zbook.protocols = zbook.protocols or {}

---@class zbook.protocols.uart
local uart = {}
zbook.protocols.uart = uart

---
---Initialize the zbook UART (checks the underlying Zephyr UART device is ready).
---
---@return integer err # 0 on success, negative errno on failure.
function uart.init() end

---
---Write bytes over the zbook UART.
---
---@param data string # Bytes to send.
---@return integer err # 0 on success, negative errno on failure.
function uart.write(data) end

---
---Read bytes from the zbook UART.
---
---@param len integer # Number of bytes to read (must be positive).
---@return integer err # 0 on success, negative errno on failure.
---@return string|nil data # Bytes read, or nil on error.
function uart.read(len) end

---
---Reconfigure the zbook UART: baud rate, parity, stop bits and data bits.
---All fields but `baudrate` are optional and default to 8N1.
---
---@param cfg { baudrate: integer, parity: ("none"|"odd"|"even")?, stop_bits: (1|2)?, data_bits: (7|8|9)? }
---@return integer err # 0 on success, negative errno on failure.
function uart.cfg(cfg) end

---
---Bind the zbook UART to a pair of PIO GPIOs (RP2350 PIO-UART backend) and
---set its baud rate. Only meaningful on boards where zbook-uart is routed
---through PIO instead of a hardware UART peripheral.
---
---@param tx_pin integer # PIO GPIO index used for TX.
---@param rx_pin integer # PIO GPIO index used for RX.
---@param baudrate integer # Baud rate in bps.
---@return integer err # 0 on success, negative errno on failure.
function uart.register_pin(tx_pin, rx_pin, baudrate) end

return uart
