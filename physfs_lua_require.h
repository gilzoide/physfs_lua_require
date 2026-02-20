#pragma once

#include <lua.h>
#include <physfs.h>

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
// Lua/C API
///////////////////////////////////////////////////////////////////////////////

/**
 * Variant for `luaL_loadfilex` that uses PhysFS to read file data.  [-0, +1, -]
 *
 * This function returns the same results as `lua_load` or `LUA_ERRFILE` for file-related errors.
 */
int physfs_luaL_loadfilex(lua_State *L, const char *filename, const char *mode);
/**
 * Equivalent to `physfs_luaL_loadfilex` with mode equal to `NULL`.  [-0, +1, -]
 */
#define physfs_luaL_loadfile(L, filename)	physfs_luaL_loadfilex((L), (filename), NULL)

/**
 * Variant for `lua_dump` that writes into a file using PhysFS.  [-0, +0, –]
 */
int physfs_lua_dumpfile(lua_State *L, const char *filename, int strip);


///////////////////////////////////////////////////////////////////////////////
// Lua functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Variant of `package.searchpath` that uses PhysFS to find files.
 */
int physfs_searchpath(lua_State *L);

/**
 * Searcher function that loads files using PhysFS.
 *
 * Lua params:
 * - name: Name of the module to load. Module will be searched using `physfs_searchpath`.
 * Lua returns:
 * - Loaded Lua script if module is found and loaded with success
 * Errors if
 */
int physfs_lua_searcher(lua_State *L);

/**
 * Replace `package.searchpath` by `physfs_searchpath`.
 * @return `LUA_OK` if everything went fine, otherwise one of `LUA_ERRRUN` or `LUA_ERRMEM`.
 */
int physfs_lua_replace_searchpath(lua_State *L);

/**
 * Replace `package.searchers[2]` (or `package.loaders[2]` in Lua 5.1) by `physfs_lua_searcher`.
 * @return `LUA_OK` if everything went fine, otherwise one of `LUA_ERRRUN` or `LUA_ERRMEM`.
 */
int physfs_lua_replace_lua_searcher(lua_State *L);

///////////////////////////////////////////////////////////////////////////////
// Requireable module
///////////////////////////////////////////////////////////////////////////////

/**
 * Requireable function that returns this module.
 *
 * @code{.lua}
 * {
 *   searchpath = physfs_searchpath,
 *   lua_searcher = physfs_lua_searcher,
 * }
 * @endcode
 */
int luaopen_physfs_lua_require(lua_State *L);

#ifdef __cplusplus
}
#endif
