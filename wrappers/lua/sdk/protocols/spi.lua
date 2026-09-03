---@meta

--- ZBook generic SPI protocol binding (zbook.protocols.spi).
--- Wraps zbook_spi_init/write/read/transceive from
--- interface/includes/protocols/zbook_spi.h.

zbook = zbook or {}
zbook.protocols = zbook.protocols or {}

---@class zbook.protocols.spi
local spi = {}
zbook.protocols.spi = spi

-- SPI clock polarity/phase combination (standard SPI modes 0-3).
--
-- | mode | CPOL | CPHA | clock idle | sample edge   |
-- | ---- | ---- | ---- | ---------- | ------------- |
-- | 0    | 0    | 0    | low        | rising edge   |
-- | 1    | 0    | 1    | low        | falling edge  |
-- | 2    | 1    | 0    | high       | falling edge  |
-- | 3    | 1    | 1    | high       | rising edge   |
--
-- Match this to the SPI device's datasheet (often listed as "SPI mode N").

---@type integer
spi.MODE_0 = 0
---@type integer
spi.MODE_1 = 1
---@type integer
spi.MODE_2 = 2
---@type integer
spi.MODE_3 = 3

-- Bit order used to shift each word over the wire (MOSI/MISO).
-- Most SPI devices are MSB_FIRST; check the datasheet if unsure.

---@type integer
spi.MSB_FIRST = 0
---@type integer
spi.LSB_FIRST = 1

---
---Initialize the zbook SPI bus (checks the underlying Zephyr SPI device is ready).
---
---@return integer err # 0 on success, negative errno on failure.
function spi.init() end

---
---Reconfigure the zbook SPI bus: clock frequency, mode (spi.MODE_0..spi.MODE_3),
---bit order (spi.MSB_FIRST / spi.LSB_FIRST) and word size in bits (e.g. 8).
---All fields are required — there is no partial update.
---
---@param cfg { frequency: integer, mode: integer, bit_order: integer, word_size: integer }
---@return integer err # 0 on success, negative errno on failure.
function spi.configure(cfg) end

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
