#pragma once

#include <lua.h>
#include <physfs.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Variant of `package.searchpath` that uses PhysFS to find files.
 */
int physfs_searchpath(lua_State *L);

/**
 * Searcher function that loads files using PhysFS.
 */
int physfs_lua_searcher(lua_State *L);

/**
 * Variant for `luaL_loadfilex` that uses PhysFS to read file data. [-0, +1, m]
 *
 * This function returns the same results as `lua_load` or `LUA_ERRFILE` for file-related errors.
 */
int physfs_luaL_loadfilex(lua_State *L, const char *filename, const char *mode);
/**
 * Equivalent to `physfs_luaL_loadfilex` with mode equal to `NULL`.
 */
#define physfs_luaL_loadfile(L, filename)	physfs_luaL_loadfilex((L), (filename), NULL)

/**
 * Variant for `lua_dump` that writes into a file using PhysFS.
 */
int physfs_lua_dumpfile(lua_State *L, const char *filename, int strip);


/**
 * Function compatible with `require` that simply returns `physfs_searchpath`.
 *
 * To replace `package.searchpath` with the PhysFS enabled variant, run the following in Lua:
 * @code{.lua}
 * package.searchpath = require("physfs_searchpath")
 * @endcode
 */
int luaopen_physfs_searchpath(lua_State *L);

/**
 * Function compatible with `require` that simply returns `physfs_lua_searcher`.
 *
 * To replace the Lua searcher with the PhysFS enabled variant, run the following in Lua:
 * @code{.lua}
 * package.searchers[2] = require("physfs_lua_searcher")
 * @endcode
 */
int luaopen_physfs_lua_searcher(lua_State *L);

#ifdef __cplusplus
}
#endif
