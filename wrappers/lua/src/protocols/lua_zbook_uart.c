
/*******************************************************************
 * @file lua_zbook_uart.c
 *
 * @brief Lua binding for the zbook UART protocol interface wrappwer.
 * @author João Matheus Nascimento Dias (joao.dias@edge.ufal.br)
 * @version 0.1
 * @date 21/08/2026
 *
 * @copyright Copyright (c) 2026
 *
 *******************************************************************/
#include <lua_zbook/protocols/lua_zbook_uart.h>
#include <protocols/zbook_uart.h>

#include <string.h>
#include <lauxlib.h>
#include <zephyr/kernel.h>

/* Lua state and callback ref for uart.on_rx(). Guarded because the UART ISR
 * (via zbook_uart_isr) runs on a different context than the Lua VM; the
 * actual Lua call is deferred to a system workqueue item. */
static lua_State *rx_L;
static int rx_cb_ref = LUA_NOREF;

static void rx_work_handler(struct k_work *work)
{
	ARG_UNUSED(work);

	if (!rx_L || rx_cb_ref == LUA_NOREF) {
		return;
	}

	lua_rawgeti(rx_L, LUA_REGISTRYINDEX, rx_cb_ref);
	if (lua_pcall(rx_L, 0, 0, 0) != LUA_OK) {
		lua_pop(rx_L, 1);
	}
}

static K_WORK_DEFINE(rx_work, rx_work_handler);

/** @brief Runs in ISR context; defers the Lua callback to the system workqueue. */
static void uart_rx_isr(const struct device *dev, void *user_data)
{
	ARG_UNUSED(dev);
	ARG_UNUSED(user_data);

	k_work_submit(&rx_work);
}

/** @brief Lua function: uart.init() -> err. */
static int l_uart_init(lua_State *L)
{
	int err = zbook_uart_init();

	lua_pushinteger(L, err);
	return 1;
}

/** @brief Lua function: uart.write(data) -> err. */
static int l_uart_write(lua_State *L)
{
	size_t len;
	const char *data = luaL_checklstring(L, 1, &len);

	int err = zbook_uart_write((const uint8_t *)data, len);

	lua_pushinteger(L, err);
	return 1;
}

/** @brief Lua function: uart.read(len) -> err, data. */
static int l_uart_read(lua_State *L)
{
	lua_Integer len = luaL_checkinteger(L, 1);

	if (len <= 0) {
		return luaL_error(L, "len must be positive");
	}

	uint8_t *buf = lua_newuserdata(L, (size_t)len);
	int err = zbook_uart_read(buf, (size_t)len);

	lua_pushinteger(L, err);
	if (err) {
		lua_pushnil(L);
	} else {
		lua_pushlstring(L, (const char *)buf, (size_t)len);
	}
	return 2;
}

/** @brief Parse the parity field of a uart.cfg() table, defaulting to "none". */
static enum zbook_uart_parity parse_parity(lua_State *L, int idx)
{
	lua_getfield(L, idx, "parity");
	const char *parity = luaL_optstring(L, -1, "none");
	enum zbook_uart_parity result = ZBOOK_UART_PARITY_NONE;

	if (strcmp(parity, "odd") == 0) {
		result = ZBOOK_UART_PARITY_ODD;
	} else if (strcmp(parity, "even") == 0) {
		result = ZBOOK_UART_PARITY_EVEN;
	} else if (strcmp(parity, "none") != 0) {
		luaL_error(L, "invalid parity '%s' (expected 'none', 'odd' or 'even')", parity);
	}

	lua_pop(L, 1);
	return result;
}

/** @brief Parse the stop_bits field of a uart.cfg() table, defaulting to 1. */
static enum zbook_uart_stop_bits parse_stop_bits(lua_State *L, int idx)
{
	lua_getfield(L, idx, "stop_bits");
	lua_Integer stop_bits = luaL_optinteger(L, -1, 1);

	lua_pop(L, 1);

	switch (stop_bits) {
	case 1:
		return ZBOOK_UART_STOP_BITS_1;
	case 2:
		return ZBOOK_UART_STOP_BITS_2;
	default:
		return luaL_error(L, "invalid stop_bits %d (expected 1 or 2)", (int)stop_bits);
	}
}

/** @brief Parse the data_bits field of a uart.cfg() table, defaulting to 8. */
static enum zbook_uart_data_bits parse_data_bits(lua_State *L, int idx)
{
	lua_getfield(L, idx, "data_bits");
	lua_Integer data_bits = luaL_optinteger(L, -1, 8);

	lua_pop(L, 1);

	switch (data_bits) {
	case 7:
		return ZBOOK_UART_DATA_BITS_7;
	case 8:
		return ZBOOK_UART_DATA_BITS_8;
	case 9:
		return ZBOOK_UART_DATA_BITS_9;
	default:
		return luaL_error(L, "invalid data_bits %d (expected 7, 8 or 9)", (int)data_bits);
	}
}

/** @brief Lua function: uart.cfg({baudrate=.., parity=.., stop_bits=.., data_bits=..}) -> err. */
static int l_uart_cfg(lua_State *L)
{
	luaL_checktype(L, 1, LUA_TTABLE);

	lua_getfield(L, 1, "baudrate");
	lua_Integer baudrate = luaL_checkinteger(L, -1);
	lua_pop(L, 1);

	struct zbook_uart_cfg cfg = {
		.baudrate = (uint32_t)baudrate,
		.parity = parse_parity(L, 1),
		.stop_bits = parse_stop_bits(L, 1),
		.data_bits = parse_data_bits(L, 1),
	};

	int err = zbook_uart_cfg(&cfg);

	lua_pushinteger(L, err);
	return 1;
}

/** @brief Lua function: uart.on_rx(fn) -> err. Registers fn as the RX callback. */
static int l_uart_on_rx(lua_State *L)
{
	luaL_checktype(L, 1, LUA_TFUNCTION);

	if (rx_cb_ref != LUA_NOREF) {
		luaL_unref(L, LUA_REGISTRYINDEX, rx_cb_ref);
	}

	rx_L = L;
	rx_cb_ref = luaL_ref(L, LUA_REGISTRYINDEX);

	int err = zbook_uart_set_callback(uart_rx_isr, NULL);

	lua_pushinteger(L, err);
	return 1;
}

static const luaL_Reg uart_wrappers[] = {
	{"init", l_uart_init},   {"write", l_uart_write}, {"read", l_uart_read},
	{"cfg", l_uart_cfg},     {"on_rx", l_uart_on_rx},
	{NULL, NULL},
};

int luaopen_zbook_uart(lua_State *L)
{
	luaL_newlib(L, uart_wrappers);
	return 1;
}
