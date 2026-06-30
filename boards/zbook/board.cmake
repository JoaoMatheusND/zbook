# Configure the zbook runners
board_runner_args(openocd --cmd-pre-init "source [find interface/cmsis-dap.cfg]")
board_runner_args(openocd --cmd-pre-init "source [find target/rp2350.cfg]")
board_runner_args(openocd --cmd-pre-init "adapter speed 4000")

include(${ZEPHYR_BASE}/boards/common/openocd.board.cmake)