/**
 * @file luaz_zbook_uart.h
 * @brief Lua binding for the zbook UART protocol interface.
 *
 * Exposes zbook_uart_* as a `uart` Lua library: init(), write(), read(), cfg(),
 * on_rx().
 */

#ifndef _LUAZ_ZBOOK_UART_H
#define _LUAZ_ZBOOK_UART_H

#include <lua.h>

/**
 * @brief Open the `uart` Lua library.
 *
 * @param L Lua state.
 * @return 1 (the library table is on the stack).
 */
int luaopen_zbook_uart(lua_State *L);

#endif /* _LUAZ_ZBOOK_UART_H */
