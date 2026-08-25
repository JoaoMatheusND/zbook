/**
 * @file zbook_lua_spi.c
 * @brief Lua binding for the zbook SPI protocol interface.
 * @author José Félix de O. Neto <josefelix.neto@edge.ufal.br>
 */

#include "protocols/zbook_lua_spi.h"

#ifdef CONFIG_ZBOOK_LUA_SPI

#include <protocols/zbook_spi.h>

#include <lauxlib.h>

#define ZBOOK_LUA_SPI_MAX_LEN                                                                      \
	256 /* Largest single spi.read()/spi.transceive() transfer accepted from Lua. */

/** @brief Lua function: spi.init() -> err. */
static int l_spi_init(lua_State *L)
{
	int err = zbook_spi_init();

	lua_pushinteger(L, err);

	return 1;
}

/** @brief Lua function: spi.write(data) -> err. */
static int l_spi_write(lua_State *L)
{
	size_t len;
	const char *data = luaL_checklstring(L, 1, &len);

	int err = zbook_spi_write((const uint8_t *)data, len);

	lua_pushinteger(L, err);

	return 1;
}

/** @brief Lua function: spi.read(len) -> err, data. */
static int l_spi_read(lua_State *L)
{
	lua_Integer len = luaL_checkinteger(L, 1);

	if (len <= 0 || len > ZBOOK_LUA_SPI_MAX_LEN) {
		return luaL_error(L, "len must be between 1 and %d", ZBOOK_LUA_SPI_MAX_LEN);
	}

	uint8_t buf[ZBOOK_LUA_SPI_MAX_LEN];
	int err = zbook_spi_read(buf, len);

	lua_pushinteger(L, err);

	if (err) {
		lua_pushnil(L);
	} else {
		lua_pushlstring(L, (const char *)buf, len);
	}

	return 2;
}

/** @brief Lua function: spi.transceive(data) -> err, data. */
static int l_spi_transceive(lua_State *L)
{
	size_t len;
	const char *tx = luaL_checklstring(L, 1, &len);

	if (len == 0 || len > ZBOOK_LUA_SPI_MAX_LEN) {
		return luaL_error(L, "len must be between 1 and %d", ZBOOK_LUA_SPI_MAX_LEN);
	}

	uint8_t rx[ZBOOK_LUA_SPI_MAX_LEN];
	int err = zbook_spi_transceive((const uint8_t *)tx, rx, len);

	lua_pushinteger(L, err);

	if (err) {
		lua_pushnil(L);
	} else {
		lua_pushlstring(L, (const char *)rx, len);
	}

	return 2;
}

static const luaL_Reg spi_wrappers[] = {
	{"init", l_spi_init}, {"write", l_spi_write},
	{"read", l_spi_read}, {"transceive", l_spi_transceive},
	{NULL, NULL},
};

int luaopen_zbook_spi(lua_State *L)
{
	luaL_newlib(L, spi_wrappers);
	return 1;
}

#endif /* CONFIG_ZBOOK_LUA_SPI */
