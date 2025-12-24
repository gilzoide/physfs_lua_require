#include "physfs_lua_require.h"

#include <luaconf.h>
#include <lauxlib.h>

#ifdef __cplusplus
extern "C" {
#endif

const char GET_SEARCH_PATH_SRC[] =
"local dirsep, execdir_repl, file_exists, string_replace = ...\n"
"local table_concat, table_insert = table.concat, table.insert\n"
"return function(name, path, sep, rep)\n"
"	sep = sep or '.'\n"
"	rep = rep or dirsep\n"
"	if sep ~= '' then\n"
"		name = string_replace(name, sep, rep)\n"
"	end\n"
"	local notfound = {}\n"
"	for template in path:gmatch('[^;]+') do\n"
"		local filename = string_replace(string_replace(template, '?', name), '!', execdir_repl)\n"
"		if file_exists(filename) then\n"
"			return filename\n"
"		else\n"
"			table_insert(notfound, ('\\n\\tno file %q'):format(filename))\n"
"		end\n"
"	end\n"
"	return nil, table_concat(notfound)\n"
"end";

// [-0, +1, -]
static int lua_physfs_file_exists(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	int exists = PHYSFS_exists(path);
	lua_pushboolean(L, exists);
	return 1;
}

// [-0, +1, -]
static int lua_string_replace(lua_State *L) {
	const char *s = luaL_checkstring(L, 1);
	const char *p = luaL_checkstring(L, 2);
	const char *r = luaL_checkstring(L, 3);
	luaL_gsub(L, s, p, r);
	return 1;
}

int physfs_searchpath(lua_State *L) {
	int args = lua_gettop(L);

	// fill registry["physfs_searchpath"] with implementation the first time this gets called
	lua_getfield(L, LUA_REGISTRYINDEX, "physfs_searchpath");
	if (lua_isnoneornil(L, -1)) {
		lua_pop(L, 1);

		int status = luaL_loadbuffer(L, GET_SEARCH_PATH_SRC, sizeof(GET_SEARCH_PATH_SRC) - 1, "physfs_searchpath");
		if (status != LUA_OK) {
			return lua_error(L);
		}

		lua_pushstring(L, LUA_DIRSEP);  // dirsep
		lua_pushstring(L, PHYSFS_getBaseDir());  // execdir_repl
		lua_pushcfunction(L, lua_physfs_file_exists);  // file_exists
		lua_pushcfunction(L, lua_string_replace);  // string_replace
		// invoke GET_SEARCH_PATH_SRC, pushing "physfs_searchpath" implementation to the stack
		lua_call(L, 4, 1);

		// registry["physfs_searchpath"] = result
		lua_pushvalue(L, -1);
		lua_setfield(L, LUA_REGISTRYINDEX, "physfs_searchpath");
	}

	lua_insert(L, 1);
	lua_call(L, args, 2);
	return 2;
}

int physfs_lua_searcher(lua_State *L) {
	lua_settop(L, 1);
	// push "package.path"
	lua_getglobal(L, "package");
	if (lua_isnoneornil(L, -1)) {
		return luaL_error(L, "package library is not opened");
	}
	lua_getfield(L, -1, "path");
	lua_remove(L, -2);

	// call `physfs_searchpath(name, package.path)`
	physfs_searchpath(L);
	if (lua_isnoneornil(L, 1)) {
		return 1;
	}

	int status = physfs_luaL_loadfile(L, lua_tostring(L, -2));
	if (status != LUA_OK) {
		return lua_error(L);
	}
	else {
		return 1;
	}
}

typedef struct PHYSFS_LuaReader {
	PHYSFS_File *file;
	char buffer[BUFSIZ];
} PHYSFS_LuaReader;

static const char *physfs_lua_read(lua_State *L, void *data, size_t *size) {
	PHYSFS_LuaReader *reader = (PHYSFS_LuaReader *) data;
	PHYSFS_sint64 result = PHYSFS_readBytes(reader->file, reader->buffer, sizeof(reader->buffer));
	if (result > 0) {
		*size = result;
		return reader->buffer;
	}
	else {
		*size = 0;
		return NULL;
	}
}

int physfs_luaL_loadfilex(lua_State *L, const char *filename, const char *mode) {
	PHYSFS_File *file = PHYSFS_openRead(filename);
	if (!file) {
		const char *what = PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
		lua_pushfstring(L, "cannot open %s: %s", filename, what);
		return LUA_ERRFILE;
	}

	struct PHYSFS_LuaReader reader;
	reader.file = file;
	int status = lua_load(L, physfs_lua_read, &reader, filename, mode);
	return status;
}

static int physfs_lua_write(lua_State *L, const void* p, size_t size, void* data) {
	PHYSFS_File *file = (PHYSFS_File *) data;
	PHYSFS_sint64 result = PHYSFS_writeBytes(file, p, size);
	if (result < 0) {
		return LUA_ERRFILE;
	}
	else {
		return LUA_OK;
	}
}

int physfs_lua_dumpfile(lua_State *L, const char *filename, int strip) {
	PHYSFS_File *file = PHYSFS_openWrite(filename);
	if (!file) {
		return LUA_ERRFILE;
	}

	int status = lua_dump(L, physfs_lua_write, file, strip);
	return status;
}

int luaopen_physfs_searchpath(lua_State *L) {
	lua_pushcfunction(L, physfs_searchpath);
	return 1;
}

int luaopen_physfs_lua_searcher(lua_State *L) {
	lua_pushcfunction(L, physfs_lua_searcher);
	return 1;
}

#ifdef __cplusplus
}
#endif
