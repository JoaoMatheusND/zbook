---@meta

--- Root ZBook Lua API table.
--- Registered as a global by the `<script>_lua_setup` hook via
--- `luaL_requiref(L, "zbook", luaopen_zbook, 1)`.
---@class zbook
zbook = zbook or {}

---@class zbook.protocols
zbook.protocols = zbook.protocols or {}

return zbook
