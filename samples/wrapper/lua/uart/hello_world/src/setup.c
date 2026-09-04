 /*******************************************************************
 * @file setup.c
 *
 * @brief Initializes the `zbook` Lua library for the uart_sample thread.
 * @author João Matheus Nascimento Dias (joao.dias@edge.ufal.br)
 * @version 0.1
 * @date 21/08/2026
 *
 * @copyright Copyright (c) 2026
 *
 *******************************************************************/
#include <lauxlib.h>
#include <lua.h>
#include <zbook_lua.h>

/** @brief Weak hook run before uart_sample.lua: registers zbook.* in package.preload. */
int uart_sample_lua_setup(lua_State *L)
{
	luaL_getsubtable(L, LUA_REGISTRYINDEX, LUA_PRELOAD_TABLE);
	lua_pushcfunction(L, luaopen_zbook);
	lua_setfield(L, -2, "zbook");
	lua_pop(L, 1);

	return 0;
}
