/*******************************************************************
 * @file lua_zbook.h
 *
 * @brief Aggregates the enabled zbook Lua bindings into a single `zbook` table.
 * @author João Matheus Nascimento Dias (joao.dias@edge.ufal.br)
 * @version 0.1
 * @date 21/08/2026
 *
 * @copyright Copyright (c) 2026
 *
 *******************************************************************/

#ifndef _LUA_ZBOOK_H
#define _LUA_ZBOOK_H

#include <lua.h>

/**
 * @brief Open the `zbook` Lua library.
 *
 * @param L Lua state.
 * @return 1 (the library table is on the stack).
 */
int luaopen_zbook(lua_State *L);

#endif /* _LUA_ZBOOK_H */
