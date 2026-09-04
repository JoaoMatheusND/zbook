/*******************************************************************
 * @file lua_zbook_spi.h
 *
 * @brief Defines the Lua Wrapper Interface for the zbook SPI bus.
 * @author José Félix de Oliveira Neto (josefelix.neto@edge.ufal.br)
 * @version 0.1
 * @date 25/08/26
 *
 * @copyright Copyright (c) 2026
 *
 *******************************************************************/
#ifndef LUA_ZBOOK_SPI_H
#define LUA_ZBOOK_SPI_H

#include <lua.h>

/**
 * @brief Open the `spi` Lua library.
 *
 * @param L Lua state.
 * @return 1 (the library table is on the stack).
 */
int luaopen_zbook_spi(lua_State *L);

#endif /* LUA_ZBOOK_SPI_H */