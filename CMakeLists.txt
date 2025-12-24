cmake_minimum_required(VERSION 3.5)

project(physfs-lua-require C CXX)

option(PHYSFS_LUA_REQUIRE_STATIC_LIB "Whether to build physfs_lua_require as static library" ON)


if(PHYSFS_LUA_REQUIRE_STATIC_LIB)
  set(_PHYSFS_LUA_REQUIRE_STATIC_OR_SHARED STATIC)
else()
  set(_PHYSFS_LUA_REQUIRE_STATIC_OR_SHARED SHARED)
endif()

# Link with this version if Lua is built as C
add_library(physfs_lua_require ${_PHYSFS_LUA_REQUIRE_STATIC_OR_SHARED} "physfs_lua_require.c" "physfs_lua_require.h")
# Link with this version if Lua is built as C++
add_library(physfs_lua_require_cxx ${_PHYSFS_LUA_REQUIRE_STATIC_OR_SHARED} "physfs_lua_require.cpp" "physfs_lua_require.h")

target_include_directories(physfs_lua_require PUBLIC ".")
target_include_directories(physfs_lua_require_cxx PUBLIC ".")

# If building PhysFS with CMake, link with built target.
# Prefer shared library over static one.
if(TARGET physfs)
  target_link_libraries(physfs_lua_require PUBLIC physfs)
  target_link_libraries(physfs_lua_require_cxx PUBLIC physfs)
elseif(TARGET physfs-static)
  target_link_libraries(physfs_lua_require PUBLIC physfs-static)
  target_link_libraries(physfs_lua_require_cxx PUBLIC physfs-static)
else()
  find_package(PhysFS REQUIRED)
  target_include_directories(physfs_lua_require PUBLIC ${PHYSFS_INCLUDE_DIR})
  target_include_directories(physfs_lua_require_cxx PUBLIC ${PHYSFS_INCLUDE_DIR})
  target_link_libraries(physfs_lua_require PUBLIC ${PHYSFS_LIBRARIES})
  target_link_libraries(physfs_lua_require_cxx PUBLIC ${PHYSFS_LIBRARIES})
endif()

# If building Lua with CMake, link with built target.
# I know, Lua doesn't have an official CMake build, but anyway if you use a custom one, just link with it.
if(TARGET lua)
  target_link_libraries(physfs_lua_require PUBLIC lua)
  target_link_libraries(physfs_lua_require_cxx PUBLIC lua)
else()
  find_package(Lua REQUIRED)
  target_include_directories(physfs_lua_require PUBLIC ${LUA_INCLUDE_DIR})
  target_include_directories(physfs_lua_require_cxx PUBLIC ${LUA_INCLUDE_DIR})
  target_link_libraries(physfs_lua_require PUBLIC ${LUA_LIBRARIES})
  target_link_libraries(physfs_lua_require_cxx PUBLIC ${LUA_LIBRARIES})
endif()
