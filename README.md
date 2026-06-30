# ZBook Board (Zephyr module)

Out-of-tree board support for the **ZBook** (Raspberry Pi **RP2350B**,
Cortex-**M33**), packaged as a [Zephyr module](https://docs.zephyrproject.org/latest/develop/modules.html).

## Contents

```
boards/zbook/                 # board definition
  zbook_rp2350b_m33.dts       #   standalone variant + partitions
  zbook_rp2350b_m33_mcuboot.dts  # MCUboot variant + partitions
  zbook_rp2350b_m33-common.dtsi  # shared hardware (both variants)
  *_defconfig / *.yaml / board.yml / board.cmake / Kconfig* / zbook-pinctrl.dtsi
boards/shields/zbook_wifi/    # attachable ESP8266 Wi-Fi shield
zephyr/module.yml             # registers boards/ as a board root
```

## Usage

Add this repo to your west manifest. `zephyr/module.yml` sets `board_root`, so
the board is discovered automatically — no `BOARD_ROOT` needed in your app's
CMake:

```yaml
# west.yml (excerpt)
projects:
  - name: board
    remote: zephyr-book
    revision: main
    path: board
```

Then build for the board target:

```bash
west build -b zbook/rp2350b/m33                       # standalone
west build -b zbook/rp2350b/m33 --shield zbook_wifi   # + Wi-Fi
west build -b zbook/rp2350b/m33/mcuboot --sysbuild    # MCUboot layout
```

## Board targets

| Target                       | Layout                                            |
|------------------------------|---------------------------------------------------|
| `zbook/rp2350b/m33`          | Standalone (boots directly), `storage` partition  |
| `zbook/rp2350b/m33/mcuboot`  | MCUboot layout, app in slot-0                      |
