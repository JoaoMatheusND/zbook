local zephyr = require("zephyr")
local zbook = require("zbook")
local string = require("string")

local spi = zbook.protocols.spi

local function to_hex(data)
	local hex = ""
	for i = 1, #data do
		if i > 1 then
			hex = hex .. " "
		end
		hex = hex .. string.format("%02x", data:byte(i))
	end
	return hex
end

local err = spi.init()
if err ~= 0 then
	zephyr.log_err("zbook_spi_init failed: " .. err)
	return
end

zephyr.printk("zbook SPI ready")

local tx = "\1\2\3\4"
local terr, rx = spi.transceive(tx)
zephyr.printk("spi.transceive err=" .. terr)
if rx then
	zephyr.printk("spi.transceive tx=" .. to_hex(tx) .. " rx=" .. to_hex(rx))
	zephyr.printk("spi.transceive loopback " .. (rx == tx and "OK (rx == tx)" or "MISMATCH (rx != tx)"))
end

err = spi.write("\1\2\3\4")
zephyr.printk("spi.write err=" .. err)

local rerr, data = spi.read(4)
zephyr.printk("spi.read err=" .. rerr)

if data then
	zephyr.printk("spi.read got " .. #data .. " bytes")
end
