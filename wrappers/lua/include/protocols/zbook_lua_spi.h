/**
 * @file zbook_lua_spi.h
 * @brief Lua binding for the zbook SPI protocol interface.
 *
 * Exposes zbook_spi_* as a `spi` Lua library: init(), write(), read() and transceive().
 */

#ifndef ZBOOK_LUA_SPI_H
#define ZBOOK_LUA_SPI_H

#include <lua.h>

/**
 * @brief Open the `spi` Lua library.
 *
 * @param L Lua state.
 * @return 1 (the library table is on the stack).
 */
int luaopen_zbook_spi(lua_State *L);

#endif /* ZBOOK_LUA_SPI_H */