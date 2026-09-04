# zbook — planner de implementação

Baseado no diagrama de arquitetura (Zephyr → Interface → Wrappers/Linguagens/Plataforma).

> Atualizado após a implementação do protocolo SPI (primeiro periférico completo,
> ponta a ponta, validado em hardware real). Vários pontos abaixo divergem do desenho
> original — a versão em código é a que vale; este arquivo foi ajustado pra bater com
> ela.

## Princípios

- **Interface é C puro, sem saber que binding existe.** `zbook_*` não importa `lua.h`,
  não sabe de nenhuma linguagem. Passthrough sobre driver Zephyr, nada mais. Pode ter
  mais que `init`/`read`/`write` quando o periférico realmente pede (ex.: SPI tem
  `zbook_spi_configure()` e `zbook_spi_transceive()` — full-duplex numa CS só é uma
  primitiva real de hardware, não luxo).
- **Binding fica em `wrappers/<lang>/`, um arquivo por periférico.** Genérico por
  linguagem — hoje só `wrappers/lua/`, mas a estrutura já assume que no futuro pode
  existir `wrappers/micropython/`, `wrappers/js/` etc, cada um espelhando a mesma árvore
  de categorias da Interface, sem depender de como o outro binding é feito.
- **Interface organizada por categoria, não plana.** Cada periférico entra numa das
  categorias abaixo — ex.: LED vai em `atuadores/`, não solto na raiz de `include/zbook/`.
  A categoria de protocolos ficou `protocols/` (inglês) no código, não `protocolos/` —
  decisão tomada ao implementar SPI, pra manter o resto do projeto em inglês. As demais
  categorias (`atuadores/`, `entrada/`, `sensores/`, `armazenamento/`) ainda não têm
  nenhum periférico implementado — confirmar/renomear quando o primeiro chegar.
- **Nome do header espelha o nome do `.c`.** `src/<categoria>/zbook_<periph>.c` tem
  header em `include/zbook/<categoria>/zbook_<periph>.h` — mesmo prefixo `zbook_` dos
  dois lados, sem header "nu" (`led.h`), pra bater 1:1 com o nome da função e do arquivo.
  Do lado do wrapper Lua o padrão ficou `lua_zbook_<periph>.c/h` (prefixo da linguagem
  primeiro), enquanto o agregador continua `zbook_lua.c/h` — ver estrutura abaixo.
- **Camada de transdução só no lado do zbook.** `../zephyr/modules/lua_zephyr` NÃO é
  modificado. Injeção via hook fraco `<script>_lua_setup(L)`, gerado automaticamente
  por `luaz_define_source_thread()` (chamado no `CMakeLists.txt` do sample, pré-`find_package(Zephyr)`)
  a partir do nome do arquivo `.lua`. O hook é chamado depois de `luaz_openlibs()` e
  antes do script rodar. **Não existe um hook único/fixo** — cada sample que define uma
  thread Lua tem seu próprio `zbook_lua_setup.c` (dentro da pasta do próprio sample,
  não em `wrappers/lua/`), implementando `<nome-do-script>_lua_setup` e chamando:
  ```c
  int <script>_lua_setup(lua_State *L) {
      luaL_requiref(L, "zbook", luaopen_zbook, 1);
      lua_pop(L, 1);
      return 0;
  }
  ```
  (resolve a decisão pendente nº1 do planner original — ver "Decisões" abaixo.)
- **Stubs de editor em `wrappers/<lang>/sdk/`.** Arquivos `---@meta` (anotação
  EmmyLua/LuaLS), corpo de função vazio, zero efeito em build/runtime — só documentam
  a API real (`zbook.<categoria>.<periph>.*`) pro language server dar autocomplete e
  type-check em quem escreve script `.lua` de verdade. Um por periférico, espelhando
  1:1 o binding real (`sdk/protocols/spi.lua` documenta exatamente o que
  `lua_zbook_spi.c` expõe), mais um agregador raiz (`sdk/zbook.lua`). Acompanha um
  `.luarc.json` em `wrappers/lua/` apontando `workspace.library` pra essa pasta (e,
  opcionalmente, pro `include/`/stub do próprio `lua_zephyr`, pra também documentar
  `zephyr.*`). Isso é só DX — nunca é compilado, nunca é embarcado no board.
- **Plataforma = placeholder para portar (Arduino IDE etc).** Não implementar agora.
  Interface deve ficar livre de qualquer detalhe de toolchain/IDE pra não pagar custo de
  retrofit depois — motivo extra pra manter `zbook_*` 100% C puro, sem dependência de
  build específico além de headers de driver Zephyr.

## Categorias (mapeadas das caixas do diagrama)

| Categoria        | Periféricos                                    |
| ---------------- | ----------------------------------------------- |
| `protocols/`     | UART, SPI ✅, I2C, AT-CMD (wifi/bt)             |
| `atuadores/`     | LED, RGB-LED, Buzzer, PWM, Display              |
| `entrada/`       | BTN, Rotary encoder, I/O, IR                    |
| `sensores/`      | ADC, LDR, TEMP-sensor, MIC, Potenciômetro       |
| `armazenamento/` | SD                                              |
| (adiado)         | IMU — sem sensor definido, categoria a decidir  |

Mesma categoria vale nos dois lados (`interface/` e cada `wrappers/<lang>/`) — path
espelhado facilita achar o par binding↔interface de qualquer periférico. `protocols/`
já é o nome real no código; as demais linhas da tabela são o plano original em
português e podem mudar pra inglês também quando implementadas (confirmar antes,
não assumir).

## Estrutura de diretórios (real, validada com SPI)

```
zbook/
  CMakeLists.txt                 # só sabe de interface/ (glob direto) + add_subdirectory(wrappers/lua)
  Kconfig                        # rsource "interface/Kconfig" + rsource "wrappers/lua/Kconfig"
  prj.conf                       # CONFIG_ZBOOK_WRAPPER_LUA=y, CONFIG_LUA=y, ...

  interface/                     # C puro, zero binding, zero linguagem
    includes/
      protocols/    zbook_spi.h  # (demais categorias/periféricos seguem o mesmo padrão)
    src/
      main.c
      protocols/    zbook_spi.c
    Kconfig                      # ZBOOK_WRAPPER_LUA (master switch) + ZBOOK_SPI (default y if ZBOOK_WRAPPER_LUA, select SPI)
    # sem CMakeLists.txt próprio — o topo faz GLOB_RECURSE direto em interface/src

  wrappers/
    lua/
      CMakeLists.txt              # GLOB_RECURSE src/*.c, include/ — chamado via add_subdirectory
      Kconfig                     # LUA_ZBOOK_SPI (default y, depends on ZBOOK_SPI && LUA)
      include/
        zbook_lua.h                # agregador: int luaopen_zbook(lua_State *L)
        protocols/
          lua_zbook_spi.h           # int luaopen_zbook_spi(lua_State *L)
      src/
        zbook_lua.c                 # luaopen_zbook(L): agrega categorias habilitadas em 1 tabela
        protocols/
          lua_zbook_spi.c            # binding: init/configure/write/read/transceive + constantes de enum
      sdk/                         # stubs ---@meta pro editor (LuaLS), não compila
        zbook.lua
        protocols/
          spi.lua
      .luarc.json                  # workspace.library: ["sdk", lua_zephyr/src]
    # futuro: wrappers/micropython/, wrappers/js/... mesma árvore, independentes

  samples/                        # ao lado de interface/ e wrappers/, não dentro de wrappers/lua/
    wrappers/
      lua/
        protocols/
          lua_spi/                 # app Zephyr standalone (binário próprio, não é o app principal)
            CMakeLists.txt          # BOARD_ROOT/ZBOOK_ROOT calculado por profundidade relativa até zbook/
            Kconfig                 # rsource até interface/Kconfig e wrappers/lua/Kconfig (mesma profundidade)
            prj.conf
            sample.yaml              # Twister, build_only:true quando depende de hardware real
            boards/
              zbook_rp2350b_m33.overlay  # fixes de devicetree específicos do sample (ex.: desligar bus conflitante)
            src/
              lua_spi.lua             # o script em si — luaz_define_source_thread(src/lua_spi.lua)
              zbook_lua_setup.c       # <nome-do-script>_lua_setup(L) -> luaL_requiref(zbook)
      # futuro: samples/wrappers/micropython/<categoria>/<sample>/, mesma profundidade
```

Header e `.c` ficam sempre com o mesmo nome de arquivo (`zbook_spi.h` / `zbook_spi.c`,
`lua_zbook_spi.h` / `lua_zbook_spi.c`) — só muda a pasta (`include`/`includes` vs `src`).

Regra de dependência: `wrappers/<lang>/**` pode incluir `interface/includes/<categoria>/*.h`.
`interface/**` nunca inclui nada de `wrappers/`. Isso é o que torna o binding plugável —
remover `wrappers/lua/` inteiro não quebra a interface, e adicionar `wrappers/js/` não
toca em nada dentro de `interface/`.

**CMake nunca precisa mudar por periférico novo.** `zbook/CMakeLists.txt` e
`wrappers/lua/CMakeLists.txt` fazem `GLOB_RECURSE` uma vez só; quem liga/desliga cada
periférico é o próprio `.c`, com o corpo inteiro (depois do `#include` do header) dentro
de `#ifdef CONFIG_ZBOOK_<PERIPH>` (interface) ou `#ifdef CONFIG_LUA_ZBOOK_<PERIPH>`
(wrapper). Arquivo desabilitado vira uma translation unit vazia — compila, não linka
nada, sem tocar em CMake. Adicionar um periférico novo é: `.h`/`.c` em cada lado + 2
linhas de `config` no Kconfig certo. Nada mais.

## Ordem de execução (por dependência, não por linha do diagrama)

1. **Base sem barramento** — LED, BTN, I/O, RGB-LED (gpio puro)
2. **Conversores simples** — ADC, PWM, Potenciômetro, LDR, TEMP-sensor
3. **Barramentos** — UART, SPI ✅, I2C (pré-requisito de tudo abaixo)
4. **Compostos sobre barramento** — Display, Buzzer, MIC, Encoder, IR
5. **Storage** — SD (sobre SPI)
6. **Rede** — AT-CMD (wifi/bluetooth) — maior complexidade, por último
7. **IMU** — bloqueado até decisão de hardware/sensor
8. **Plataforma** — não entra neste ciclo (fase futura de port pra Arduino IDE)

Dentro de cada item, ordem de arquivos (validada com SPI):
`interface/includes/<categoria>/zbook_<periph>.h` → nó devicetree `zbook_<periph>`
(se for periférico de barramento, ligado sob o controlador certo, ex. `&spi1`) →
`interface/src/<categoria>/zbook_<periph>.c` (guardado em `#ifdef CONFIG_ZBOOK_<PERIPH>`) →
2 linhas de Kconfig em `interface/Kconfig` →
`wrappers/lua/include/<categoria>/lua_zbook_<periph>.h` →
`wrappers/lua/src/<categoria>/lua_zbook_<periph>.c` (guardado em
`#ifdef CONFIG_LUA_ZBOOK_<PERIPH>`, registra constantes de enum se houver) →
2 linhas de Kconfig em `wrappers/lua/Kconfig` → registrar em `zbook_lua.c` →
stub `wrappers/lua/sdk/<categoria>/<periph>.lua` →
sample em `samples/wrappers/lua/<categoria>/lua_<periph>/` (app Zephyr standalone,
`sample.yaml` com `build_only:true` se depender de hardware) → testar no board de
verdade → só então próximo periférico.

## Padrão de código

**`interface/includes/<categoria>/zbook_<periph>.h`**

```c
int zbook_<periph>_init(void);
int zbook_<periph>_read(...);   /* conforme a caixa do diagrama; pode ter mais funções
                                    se o periférico pedir (ex.: configure/transceive) */
```

Retorno: `int` (0 = ok, `-errno` no erro), convenção Zephyr. Sem lógica de negócio, só
passthrough pro driver (`DEVICE_DT_GET`, `gpio_pin_*`, `adc_read`, `spi_transceive_dt`,
etc). Nenhum include de `lua.h` ou qualquer header de wrapper. Se o periférico tiver
opções (modo, frequência, etc.), usar `enum`s dedicados + uma struct de config e uma
função `zbook_<periph>_configure(const struct zbook_<periph>_cfg *cfg)` — ver
`zbook_spi.h` como referência real.

**`interface/src/<categoria>/zbook_<periph>.c`**

Corpo inteiro (depois do `#include "<categoria>/zbook_<periph>.h"`) dentro de
`#ifdef CONFIG_ZBOOK_<PERIPH>` / `#endif`, incluindo os `#include`s de driver Zephyr —
assim o arquivo compila como translation unit vazia quando o Kconfig está desligado,
sem precisar de nenhum `target_sources_ifdef` no CMake.

**`wrappers/lua/include/<categoria>/lua_zbook_<periph>.h`**

```c
int luaopen_zbook_<periph>(lua_State *L);
```

**`wrappers/lua/src/<categoria>/lua_zbook_<periph>.c`**

```c
#include "<categoria>/lua_zbook_<periph>.h"

#ifdef CONFIG_LUA_ZBOOK_<PERIPH>

#include <protocols/zbook_<periph>.h>
#include <lauxlib.h>

static int l_<periph>_read(lua_State *L) {
    int val;
    int err = zbook_<periph>_read(&val);
    lua_pushinteger(L, err);
    if (err) { lua_pushnil(L); } else { lua_pushinteger(L, val); }
    return 2;
}

static const luaL_Reg <periph>_wrappers[] = {
    {"read", l_<periph>_read},
    {NULL, NULL}
};

int luaopen_zbook_<periph>(lua_State *L) {
    luaL_newlib(L, <periph>_wrappers);
    /* se houver enums de opção, empurrar como constantes aqui:
     * lua_pushinteger(L, ZBOOK_<PERIPH>_FOO); lua_setfield(L, -2, "FOO"); */
    return 1;
}

#endif /* CONFIG_LUA_ZBOOK_<PERIPH> */
```

Corpo inteiro guardado em `#ifdef CONFIG_LUA_ZBOOK_<PERIPH>`, mesmo raciocínio da
interface. Constantes de enum viram inteiros na própria tabela Lua (ex.: `spi.MODE_0`),
nunca strings soltas — o valor vem direto do enum C, sem tradução manual duplicada.

**`wrappers/lua/src/zbook_lua.c`** — agregador, nesting em dois níveis (categoria → periférico):

```c
#ifdef CONFIG_LUA_ZBOOK_SPI
#include <protocols/lua_zbook_spi.h>
#endif

#if defined(CONFIG_LUA_ZBOOK_SPI)
static void push_category_protocols(lua_State *L) {
    lua_newtable(L);
    luaopen_zbook_spi(L); lua_setfield(L, -2, "spi");
    /* ... um bloco por periférico da categoria ... */
}
#endif

int luaopen_zbook(lua_State *L) {
    lua_newtable(L);
#if defined(CONFIG_LUA_ZBOOK_SPI)
    push_category_protocols(L); lua_setfield(L, -2, "protocols");
#endif
    /* ... um bloco por categoria habilitada ... */
    return 1;
}
```

**Hook de setup — vive no sample, não em `wrappers/lua/`:**

```c
/* samples/wrappers/lua/<categoria>/<sample>/src/zbook_lua_setup.c */
#include <lauxlib.h>
#include <zbook_lua.h>

int <nome_do_script>_lua_setup(lua_State *L) {
    luaL_requiref(L, "zbook", luaopen_zbook, 1);
    lua_pop(L, 1);
    return 0;
}
```

O nome da função é derivado do nome do arquivo `.lua` por `luaz_define_source_thread()`
(ex.: `src/lua_spi.lua` → `lua_spi_lua_setup`). Script Lua acessa via `zbook` como
variável global direta (por causa do `glb=1` acima) ou via `require("zbook")` — as duas
formas funcionam, já que `luaL_requiref` registra em `package.loaded` de qualquer jeito.

## Kconfig

Sem `Kconfig.zbook` nem `config ZBOOK` guarda-chuva — o cascateamento real é:

```
zbook/Kconfig
  rsource "interface/Kconfig"      # ZBOOK_WRAPPER_LUA (master switch) + ZBOOK_<PERIPH> (default y if ZBOOK_WRAPPER_LUA)
  rsource "wrappers/lua/Kconfig"   # LUA_ZBOOK_<PERIPH> (default y, depends on ZBOOK_<PERIPH> && LUA)
```

`prj.conf` só precisa de `CONFIG_ZBOOK_WRAPPER_LUA=y` + `CONFIG_LUA=y` — os
`CONFIG_ZBOOK_<PERIPH>` e `CONFIG_LUA_ZBOOK_<PERIPH>` de cada periférico já habilitado
vêm de brinde via `default y`, sem precisar listar cada um. Pra desligar só um
periférico específico, setar `CONFIG_ZBOOK_<PERIPH>=n` explicitamente sobrescreve o
default.

Cada sample standalone (em `samples/wrappers/lua/...`) tem seu próprio `Kconfig`
espelhando o boilerplate do `zbook/Kconfig` (menu Zephyr + módulo de log), com
`rsource` apontando de volta pros dois arquivos acima via caminho relativo — sem isso
os `CONFIG_ZBOOK_<PERIPH>`/`CONFIG_LUA_ZBOOK_<PERIPH>` não existem na árvore de Kconfig
do sample e qualquer linha no `prj.conf` dele é silenciosamente ignorada (mesmo aviso
de "unsatisfied dependencies" que aparece por engano no `merge_config` quando o
Kconfig não é fonte).

## Testes

- Interface: sample C puro por periférico (sem Lua), valida contra hardware/HAL antes do
  binding existir.
- Wrapper: app Zephyr **standalone** (binário próprio, não entra no app principal) em
  `samples/wrappers/lua/<categoria>/<sample>/` — `CMakeLists.txt` + `Kconfig` + `prj.conf`
  + `sample.yaml` (Twister) + `src/<script>.lua` + `src/zbook_lua_setup.c`, mesmo formato
  dos samples de `lua_zephyr`. `sample.yaml` usa `build_only: true` quando o periférico
  depende de hardware real (não dá pra validar SPI/I2C/etc. de verdade em QEMU) —
  Twister ainda serve como smoke test de compilação. Overlay de devicetree específico
  do sample (`boards/<board>.overlay`) resolve conflitos de pino ou fixups sem tocar no
  board compartilhado.

## Decisões

Resolvidas ao implementar SPI:

1. ~~`require("zbook")` via preload minimalista vs global direto?~~ **Global direto**:
   `luaL_requiref(L, "zbook", luaopen_zbook, 1)` (glb=1) — expõe `zbook` como global E
   via `require("zbook")` ao mesmo tempo, sem precisar de `package.preload` manual.
2. ~~`luaz_zbook_setup.c` — hook por script ou nome fixo?~~ **Hook por script**: cada
   sample tem seu próprio `zbook_lua_setup.c` implementando `<nome-do-script>_lua_setup`
   (nome derivado automaticamente pelo `luaz_define_source_thread()`). Sem
   duplicação de lógica real — o corpo é sempre as mesmas 3 linhas de `luaL_requiref`.

Ainda pendentes:

1. IMU — qual sensor/chip escolhido, ou fica fora do escopo atual? Qual categoria recebe?
2. AT-CMD — qual chip wifi/bt do board (define se é modem subsys, netif, ou AT sobre UART)?
