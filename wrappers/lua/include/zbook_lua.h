/*******************************************************************
 * @file zbook_lua.h
 *
 * @brief Defines the Lua Wrapper Interface for the zbook library.
 * @author José Félix de Oliveira Neto (josefelix.neto@edge.ufal.br)
 * @author João Matheus Nascimento Dias (joao.dias@edge.ufal.br)
 * @version 0.1
 * @date 25/08/26
 *
 * @copyright Copyright (c) 2026
 *
 *******************************************************************/
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
