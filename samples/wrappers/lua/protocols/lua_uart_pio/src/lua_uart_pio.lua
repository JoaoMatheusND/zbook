local zephyr = require("zephyr")
local zbook = require("zbook")

local uart = zbook.protocols.uart

-- Bit-banged UART over PIO: bypasses the devicetree-selected zbook-uart
-- peripheral and drives GPIO18 (TX) / GPIO19 (RX) via pio0 instead. Bridge
-- P18 to P19 with a jumper wire for the read() below to echo the write().
local TX_PIN = 18
local RX_PIN = 19
local BAUDRATE = 115200
local READ_SIZE = 12

local err = uart.register_pin(TX_PIN, RX_PIN, BAUDRATE)
zephyr.printk("uart.register_pin err=" .. err)
if err ~= 0 then
	return
end

err = uart.write("Hello, world!\n")
zephyr.printk("uart.write err=" .. err)

local rerr, data = uart.read(READ_SIZE)
zephyr.printk("uart.read err=" .. rerr)
if data then
	zephyr.printk("uart.read got " .. #data .. " bytes: " .. data)
end
