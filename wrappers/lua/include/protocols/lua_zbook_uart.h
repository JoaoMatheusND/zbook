/*******************************************************************
 * @file lua_zbook_uart.h
 *
 * @brief Defines the Lua Wrapper Interface for the zbook UART bus.
 * @author João Matheus Nascimento Dias (joao.dias@edge.ufal.br)
 * @version 0.1
 * @date 21/08/2026
 *
 * @copyright Copyright (c) 2026
 *
 *******************************************************************/
#ifndef LUA_ZBOOK_UART_H
#define LUA_ZBOOK_UART_H

#include <lua.h>

/**
 * @brief Open the `uart` Lua library.
 *
 * @param L Lua state.
 * @return 1 (the library table is on the stack).
 */
int luaopen_zbook_uart(lua_State *L);

#endif /* LUA_ZBOOK_UART_H */
