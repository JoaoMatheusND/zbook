/**
 * @file zbook_lua.h
 * @brief Aggregates the enabled zbook Lua bindings into a single `zbook` table.
 */

#ifndef ZBOOK_LUA_H
#define ZBOOK_LUA_H

#include <lua.h>

/**
 * @brief Open the `zbook` Lua library.
 *
 * @param L Lua state.
 * @return 1 (the library table is on the stack).
 */
int luaopen_zbook(lua_State *L);

#endif /* ZBOOK_LUA_H */