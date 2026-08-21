# zbook — planner de implementação

Baseado no diagrama de arquitetura (Zephyr → Interface → Wrappers/Linguagens/Plataforma).

## Princípios

- **Interface é C puro, sem saber que binding existe.** `zbook_*` não importa `lua.h`,
  não sabe de nenhuma linguagem. Passthrough sobre driver Zephyr, nada mais.
- **Binding fica em `wrappers/<lang>/`, um arquivo por periférico.** Genérico por
  linguagem — hoje só `wrappers/lua/`, mas a estrutura já assume que no futuro pode
  existir `wrappers/micropython/`, `wrappers/js/` etc, cada um espelhando a mesma árvore
  de categorias da Interface, sem depender de como o outro binding é feito.
- **Interface organizada por categoria, não plana.** Cada periférico entra numa das
  categorias abaixo — ex.: LED vai em `atuadores/`, não solto na raiz de `include/zbook/`.
- **Nome do header espelha o nome do `.c`.** `src/<categoria>/zbook_<periph>.c` tem
  header em `include/zbook/<categoria>/zbook_<periph>.h` — mesmo prefixo `zbook_` dos
  dois lados, sem header "nu" (`led.h`), pra bater 1:1 com o nome da função e do arquivo.
- **Camada de transdução só no lado do zbook.** `../zephyr/modules/lua_zephyr` NÃO é
  modificado. Injeção via hook fraco `<script>_lua_setup(L)` (gerado por
  `luaz_define_*_thread`, ver `templates/lua_thread.c.in`), chamado depois de
  `luaz_openlibs()` e antes do script rodar. Cada thread zbook registra sua lib com
  `luaL_requiref(L, "zbook", luaopen_zbook, 1)` dentro desse hook.
- **Plataforma = placeholder para portar (Arduino IDE etc).** Não implementar agora.
  Interface deve ficar livre de qualquer detalhe de toolchain/IDE pra não pagar custo de
  retrofit depois — motivo extra pra manter `zbook_*` 100% C puro, sem dependência de
  build específico além de headers de driver Zephyr.

## Categorias (mapeadas das caixas do diagrama)

| Categoria        | Periféricos                                    |
| ---------------- | ----------------------------------------------- |
| `protocolos/`    | UART, SPI, I2C, AT-CMD (wifi/bt)                |
| `atuadores/`     | LED, RGB-LED, Buzzer, PWM, Display              |
| `entrada/`       | BTN, Rotary encoder, I/O, IR                    |
| `sensores/`      | ADC, LDR, TEMP-sensor, MIC, Potenciômetro       |
| `armazenamento/` | SD                                              |
| (adiado)         | IMU — sem sensor definido, categoria a decidir  |

Mesma categoria vale nos dois lados (`interface/` e cada `wrappers/<lang>/`) — path
espelhado facilita achar o par binding↔interface de qualquer periférico.

## Estrutura de diretórios (nova, dentro deste repo)

```
zbook/                          # componente na raiz do repo, ao lado de src/
  CMakeLists.txt                # agrega interface/ e wrappers/
  Kconfig.zbook                 # agrega Kconfig por categoria + por wrapper

  interface/                    # C puro, zero binding, zero linguagem
    include/zbook/
      protocolos/     zbook_uart.h zbook_spi.h zbook_i2c.h zbook_at_cmd.h
      atuadores/       zbook_led.h zbook_rgb_led.h zbook_buzzer.h zbook_pwm.h zbook_display.h
      entrada/         zbook_btn.h zbook_encoder.h zbook_io.h zbook_ir.h
      sensores/        zbook_adc.h zbook_ldr.h zbook_temp.h zbook_mic.h zbook_pot.h  (imu — adiado)
      armazenamento/   zbook_sd.h
    src/
      protocolos/      zbook_uart.c zbook_spi.c zbook_i2c.c zbook_at_cmd.c
      atuadores/       zbook_led.c zbook_rgb_led.c zbook_buzzer.c zbook_pwm.c zbook_display.c
      entrada/         zbook_btn.c zbook_encoder.c zbook_io.c zbook_ir.c
      sensores/        zbook_adc.c zbook_ldr.c zbook_temp.c zbook_mic.c zbook_pot.c
      armazenamento/   zbook_sd.c
    Kconfig              # CONFIG_ZBOOK_<PERIPH> por módulo, 1 source por categoria
    CMakeLists.txt

  wrappers/
    lua/
      include/luaz_zbook/
        protocolos/     luaz_zbook_uart.h luaz_zbook_spi.h luaz_zbook_i2c.h luaz_zbook_at_cmd.h
        atuadores/       luaz_zbook_led.h luaz_zbook_rgb_led.h luaz_zbook_buzzer.h ...
        entrada/         luaz_zbook_btn.h luaz_zbook_encoder.h luaz_zbook_io.h luaz_zbook_ir.h
        sensores/        luaz_zbook_adc.h luaz_zbook_ldr.h luaz_zbook_temp.h ...
        armazenamento/   luaz_zbook_sd.h
      src/
        protocolos/      luaz_zbook_uart.c luaz_zbook_spi.c luaz_zbook_i2c.c luaz_zbook_at_cmd.c
        atuadores/       luaz_zbook_led.c luaz_zbook_rgb_led.c luaz_zbook_buzzer.c ...
        entrada/         luaz_zbook_btn.c luaz_zbook_encoder.c luaz_zbook_io.c luaz_zbook_ir.c
        sensores/        luaz_zbook_adc.c luaz_zbook_ldr.c luaz_zbook_temp.c ...
        armazenamento/   luaz_zbook_sd.c
        luaz_zbook.c        # luaopen_zbook(L): agrega categorias habilitadas em 1 tabela
                            # (zbook.protocolos.uart, zbook.atuadores.led, ...)
        luaz_zbook_setup.c  # implementa <script>_lua_setup, chama luaL_requiref
      Kconfig               # CONFIG_ZBOOK_LUA_<PERIPH>, depends on CONFIG_ZBOOK_<PERIPH>
      CMakeLists.txt
    # futuro: wrappers/micropython/, wrappers/js/... mesma árvore por categoria, independentes
```

Header e `.c` ficam sempre com o mesmo nome de arquivo (`zbook_led.h` / `zbook_led.c`,
`luaz_zbook_led.h` / `luaz_zbook_led.c`) — só muda a pasta (`include/` vs `src/`).

Regra de dependência: `wrappers/<lang>/**` pode incluir `interface/include/zbook/<categoria>/*.h`.
`interface/**` nunca inclui nada de `wrappers/`. Isso é o que torna o binding plugável —
remover `wrappers/lua/` inteiro não quebra a interface, e adicionar `wrappers/js/` não
toca em nada dentro de `interface/`.

## Ordem de execução (por dependência, não por linha do diagrama)

1. **Base sem barramento** — LED, BTN, I/O, RGB-LED (gpio puro)
2. **Conversores simples** — ADC, PWM, Potenciômetro, LDR, TEMP-sensor
3. **Barramentos** — UART, SPI, I2C (pré-requisito de tudo abaixo)
4. **Compostos sobre barramento** — Display, Buzzer, MIC, Encoder, IR
5. **Storage** — SD (sobre SPI)
6. **Rede** — AT-CMD (wifi/bluetooth) — maior complexidade, por último
7. **IMU** — bloqueado até decisão de hardware/sensor
8. **Plataforma** — não entra neste ciclo (fase futura de port pra Arduino IDE)

Dentro de cada item, ordem de arquivos:
`interface/include/zbook/<categoria>/zbook_<periph>.h` →
`interface/src/<categoria>/zbook_<periph>.c` → testar isolado (sample C puro, sem Lua) →
`wrappers/lua/include/luaz_zbook/<categoria>/luaz_zbook_<periph>.h` →
`wrappers/lua/src/<categoria>/luaz_zbook_<periph>.c` → registrar em `luaz_zbook.c` →
sample `.lua` + `sample.yaml` (twister) → só então próximo periférico.

## Padrão de código

**`interface/include/zbook/<categoria>/zbook_<periph>.h`**

```c
int zbook_<periph>_init(void);
int zbook_<periph>_read(...);   /* conforme a caixa do diagrama */
```

Retorno: `int` (0 = ok, `-errno` no erro), convenção Zephyr. Sem lógica de negócio, só
passthrough pro driver (`DEVICE_DT_GET`, `gpio_pin_*`, `adc_read`, etc). Nenhum include
de `lua.h` ou qualquer header de wrapper.

**`wrappers/lua/include/luaz_zbook/<categoria>/luaz_zbook_<periph>.h`**

```c
int luaopen_zbook_<periph>(lua_State *L);
```

**`wrappers/lua/src/<categoria>/luaz_zbook_<periph>.c`** — mesmo padrão de `luaz_zbus.c`:

```c
#include <zbook/<categoria>/zbook_<periph>.h>
#include <luaz_zbook/<categoria>/luaz_zbook_<periph>.h>
#include <lauxlib.h>

static int l_<periph>_read(lua_State *L) {
    int val;
    int err = zbook_<periph>_read(&val);
    if (err) return luaL_error(L, "zbook_<periph>_read failed: %d", err);
    lua_pushinteger(L, val);
    return 1;
}
static const luaL_Reg <periph>_wrappers[] = {
    {"read", l_<periph>_read},
    {NULL, NULL}
};
int luaopen_zbook_<periph>(lua_State *L) {
    luaL_newlib(L, <periph>_wrappers);
    return 1;
}
```

**`wrappers/lua/src/luaz_zbook.c`** — agregador, nesting em dois níveis (categoria → periférico):

```c
static void push_category_atuadores(lua_State *L) {
    lua_newtable(L);
#ifdef CONFIG_ZBOOK_LUA_LED
    luaopen_zbook_led(L); lua_setfield(L, -2, "led");
#endif
    /* ... um bloco por periférico da categoria ... */
}

int luaopen_zbook(lua_State *L) {
    lua_newtable(L);
#ifdef CONFIG_ZBOOK_LUA_ATUADORES
    push_category_atuadores(L); lua_setfield(L, -2, "atuadores");
#endif
    /* ... um bloco por categoria habilitada ... */
    return 1;
}
```

**`wrappers/lua/src/luaz_zbook_setup.c`** — hook por thread gerada:

```c
int __weak my_script_lua_setup(lua_State *L) {
    luaL_requiref(L, "zbook", luaopen_zbook, 1);
    lua_pop(L, 1);
    return 0;
}
```

Script Lua acessa via `local zbook = require("zbook")`, depois `zbook.atuadores.led.on()` —
precisa registrar `"zbook"` em `package.preload` (mesmo require minimalista de
`luaz_utils.c`) OU usar `luaL_requiref` com `glb=1` que já seta variável global `zbook`,
dispensando `require()`. Decisão pendente abaixo.

## Kconfig

`Kconfig.zbook` na raiz de `zbook/`:

```
config ZBOOK
    bool "Enable ZBook peripheral interface"
menu "ZBook peripherals"
    depends on ZBOOK
    source "zbook/interface/Kconfig"     # 1 CONFIG_ZBOOK_<PERIPH> por módulo, agrupado por categoria
endmenu
menu "ZBook language wrappers"
    depends on ZBOOK
    source "zbook/wrappers/lua/Kconfig"  # 1 CONFIG_ZBOOK_LUA_<PERIPH>, depends on LUA
endmenu
```

## Testes

- Interface: sample C puro por periférico (sem Lua), valida contra hardware/HAL antes do
  binding existir.
- Wrapper: sample twister em `zbook/wrappers/lua/samples/<categoria>/lua_<periph>/` com
  `sample.yaml` + script Lua chamando `zbook.<categoria>.<periph>.<fn>` — mesmo formato
  dos samples de `lua_zephyr`.

## Decisões pendentes (bloqueiam início do código)

1. `require("zbook")` via preload minimalista vs global direto (`luaL_requiref` glb=1)?
2. IMU — qual sensor/chip escolhido, ou fica fora do escopo atual? Qual categoria recebe?
3. AT-CMD — qual chip wifi/bt do board (define se é modem subsys, netif, ou AT sobre UART)?
4. `luaz_zbook_setup.c` — um hook por script gerado, ou nome fixo conhecido (ex.: sempre
   `app_lua_setup`) pra não duplicar código por sample?
