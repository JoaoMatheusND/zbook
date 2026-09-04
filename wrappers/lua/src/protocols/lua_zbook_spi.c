/*******************************************************************
 * @file lua_zbook_spi.c
 *
 * @brief Implements the Lua Wrapper Interface for the zbook SPI bus.
 * @author José Félix de Oliveira Neto (josefelix.neto@edge.ufal.br)
 * @version 0.1
 * @date 25/08/26
 *
 * @copyright Copyright (c) 2026
 *
 *******************************************************************/

#include "protocols/lua_zbook_spi.h"

#ifdef CONFIG_LUA_ZBOOK_SPI

#include <lauxlib.h>
#include <protocols/zbook_spi.h>

#define ZBOOK_LUA_SPI_MAX_LEN 256
/*
 * Note: Largest single spi.read()/spi.transceive() transfer accepted from Lua.
 */

/** @brief Lua function: spi.init().
 *
 * @return 0 on success, -errno on error.
 */
static int l_spi_init(lua_State *L)
{
	int err = zbook_spi_init();

	lua_pushinteger(L, err);

	return 1;
}

/**
 * @brief Lua function: spi.configure(cfg).
 *
 * @param cfg Table with the following fields:
 *   - frequency: Clock frequency in Hz.
 *   - mode: CPOL/CPHA combination (0-3).
 *   - bit_order: MSB or LSB first (0-1).
 *   - word_size: Bits per word (e.g. 8).
 * @return 0 on success, -errno on error.
 */
static int l_spi_configure(lua_State *L)
{
	luaL_checktype(L, 1, LUA_TTABLE);

	struct zbook_spi_cfg cfg = {0};

	lua_getfield(L, 1, "frequency");
	cfg.frequency = (uint32_t)luaL_checkinteger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, 1, "mode");
	cfg.mode = (enum zbook_spi_mode)luaL_checkinteger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, 1, "bit_order");
	cfg.bit_order = (enum zbook_spi_bit_order)luaL_checkinteger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, 1, "word_size");
	lua_Integer word_size = luaL_checkinteger(L, -1);
	lua_pop(L, 1);

	if (word_size <= 0 || word_size > 63) {
		return luaL_error(L, "word_size must be between 1 and 63");
	}

	cfg.word_size = (uint8_t)word_size;

	int err = zbook_spi_configure(&cfg);

	lua_pushinteger(L, err);

	return 1;
}

/**
 * @brief Lua function: spi.write(data).
 *
 * @param data String containing the bytes to send.
 * @return 0 on success, -errno on error.
 */
static int l_spi_write(lua_State *L)
{
	size_t len;
	const char *data = luaL_checklstring(L, 1, &len);

	int err = zbook_spi_write((const uint8_t *)data, len);

	lua_pushinteger(L, err);

	return 1;
}

/**
 * @brief Lua function: spi.read(len)
 *
 * @param len Number of bytes to read.
 * @return err, data. On error, data is nil.
 */
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

/**
 * @brief Lua function: spi.transceive(data).
 *
 * @param data String containing the bytes to send.
 * @return err, data. On error, data is nil.
 */
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
	{"init", l_spi_init}, {"configure", l_spi_configure},   {"write", l_spi_write},
	{"read", l_spi_read}, {"transceive", l_spi_transceive}, {NULL, NULL},
};

int luaopen_zbook_spi(lua_State *L)
{
	luaL_newlib(L, spi_wrappers);

	lua_pushinteger(L, ZBOOK_SPI_MODE_0);
	lua_setfield(L, -2, "MODE_0");
	lua_pushinteger(L, ZBOOK_SPI_MODE_1);
	lua_setfield(L, -2, "MODE_1");
	lua_pushinteger(L, ZBOOK_SPI_MODE_2);
	lua_setfield(L, -2, "MODE_2");
	lua_pushinteger(L, ZBOOK_SPI_MODE_3);
	lua_setfield(L, -2, "MODE_3");

	lua_pushinteger(L, ZBOOK_SPI_MSB_FIRST);
	lua_setfield(L, -2, "MSB_FIRST");
	lua_pushinteger(L, ZBOOK_SPI_LSB_FIRST);
	lua_setfield(L, -2, "LSB_FIRST");

	return 1;
}

#endif /* CONFIG_LUA_ZBOOK_SPI */
