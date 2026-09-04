/* Linker snippet for the zbook_lua_protocol iterable section (see
 * include/zbook_lua_protocol.h). Registered via zephyr_linker_sources()
 * so ZBOOK_LUA_PROTOCOL_DEFINE() entries land in one contiguous,
 * STRUCT_SECTION_FOREACH()-iterable region. */

#include <zephyr/linker/iterable_sections.h>

ITERABLE_SECTION_RAM(zbook_lua_protocol, Z_LINK_ITERABLE_SUBALIGN)
