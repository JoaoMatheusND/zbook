---@meta

--- ZBook generic SPI protocol binding (zbook.protocols.spi).
--- Wraps zbook_spi_init/write/read/transceive from
--- interface/includes/protocols/zbook_spi.h.

zbook = zbook or {}
zbook.protocols = zbook.protocols or {}

---@class zbook.protocols.spi
local spi = {}
zbook.protocols.spi = spi

---
---Initialize the zbook SPI bus (checks the underlying Zephyr SPI device is ready).
---
---@return integer err # 0 on success, negative errno on failure.
function spi.init() end

---
---Write bytes over the zbook SPI bus.
---
---@param data string # Bytes to send.
---@return integer err # 0 on success, negative errno on failure.
function spi.write(data) end

---
---Read bytes from the zbook SPI bus.
---
---@param len integer # Number of bytes to read (1-256).
---@return integer err # 0 on success, negative errno on failure.
---@return string|nil data # Bytes read, or nil on error.
function spi.read(len) end

---
---Exchange bytes over the zbook SPI bus in a single transaction (one CS
---assertion). Prefer this over separate write()+read() calls when talking
---to a real SPI device that expects command+response within one CS cycle
---(sensors, displays, ...).
---
---@param data string # Bytes to send (1-256).
---@return integer err # 0 on success, negative errno on failure.
---@return string|nil data # Bytes received (same length as sent), or nil on error.
function spi.transceive(data) end

return spi
