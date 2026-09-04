 /*******************************************************************
 * @file zbook_lua_setup.c
 *
 * @brief Registers the `zbook` Lua library for the lua_uart sample thread.
 * @author João Matheus Nascimento Dias (joao.dias@edge.ufal.br)
 * @version 0.1
 * @date 21/08/2026
 *
 * @copyright Copyright (c) 2026
 *
 *******************************************************************/

#include <lauxlib.h>
#include <zbook_lua.h>

int lua_uart_lua_setup(lua_State *L)
{
	luaL_requiref(L, "zbook", luaopen_zbook, 1);
	lua_pop(L, 1);

	return 0;
}
