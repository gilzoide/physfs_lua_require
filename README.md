# PhysFS Lua Require
Simple implementation for requiring [Lua](https://www.lua.org/) modules using [PhysFS](https://github.com/icculus/physfs).


## Features
- Lua/C API
  + Use `physfs_luaL_loadfilex`/`physfs_luaL_loadfile` instead of `luaL_loadfilex`/`physfs_luaL_loadfile` for loading Lua files using PhysFS
  + Use `physfs_lua_dumpfile` as an alternative to `lua_dump` to write to file using PhysFS
- Requireable function `luaopen_physfs_lua_require` to help inject PhysFS Lua loader into Lua's `package.searchers`
  ```lua
  local physfs_lua_require = require("physfs_lua_require")
  package.searchpath = physfs_lua_require.searchpath
  package.searchers[2] = physfs_lua_require.lua_searcher
  ```
- Easy to build: just compile and link `physfs_lua_require.c` (or `physfs_lua_require.cpp` if compiling Lua as C++)
- CMake build script to easily integration with existing CMake builds
  ```cmake
  add_subdirectory(physfs_lua_require)
  target_link_libraries(my_awesome_target PRIVATE physfs_lua_require)
  ## If building Lua as C++, link with `physfs_lua_require_cxx` instead
  # target_link_libraries(my_awesome_target PRIVATE physfs_lua_require_cxx)
  ```
  + If building PhysFS and/or Lua using CMake, `physfs_lua_require` will link with the CMake targets instead of searching for the libraries in your system.
