local uart = require("zbook").protocols.uart

local err = uart.cfg({
	baudrate = 115200,
	parity = "none",
	stop_bits = 1,
	data_bits = 8,
})

err = uart.init()

err = uart.write("Hello, world!\\n")