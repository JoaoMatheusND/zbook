local zephyr = require("zephyr")
local zbook = require("zbook")

local uart = zbook.protocols.uart

local err = uart.cfg({
	baudrate = 115200,
	parity = "none",
	stop_bits = 1,
	data_bits = 8,
})
zephyr.printk("uart.cfg  err=" .. err)

err = uart.init()
zephyr.printk("uart.init err=" .. err)

err = uart.write("Hello, world!\n")
zephyr.printk("uart.write err=" .. err)
