/**
 * @file luaz_zbook.h
 * @brief Aggregates the enabled zbook Lua bindings into a single `zbook` table.
 */

#ifndef _LUAZ_ZBOOK_H
#define _LUAZ_ZBOOK_H

#include <lua.h>

/**
 * @brief Open the `zbook` Lua library.
 *
 * @param L Lua state.
 * @return 1 (the library table is on the stack).
 */
int luaopen_zbook(lua_State *L);

#endif /* _LUAZ_ZBOOK_H */
