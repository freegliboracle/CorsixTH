/*
Copyright (c) 2010 Peter "Corsix" Cawley

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef CORSIX_TH_TH_LUA_INTERNAL_H_
#define CORSIX_TH_TH_LUA_INTERNAL_H_
#include "config.h"
#include "th_lua.h"
#ifndef _MSC_VER
#define stricmp strcasecmp
#else
#pragma warning(disable: 4996) // Deprecated CRT
#endif

enum eTHLuaMetatable
{
    MT_Map,
    MT_Palette,
    MT_Sheet,
    MT_Font,
    MT_Layers,
    MT_Anims,
    MT_Anim,
    MT_Path,
    MT_Surface,
    MT_Bitmap,
    MT_Cursor,
    MT_SoundArc,
    MT_SoundFx,
    MT_String,
    MT_WindowBase,

    MT_Count,

    MT_DummyString,
};

struct THLuaRegisterState_t
{
    lua_State *L;
    int aiMetatables[MT_Count];
    int iMainTable;
    int iTop;
};

void luaT_setclosure(const THLuaRegisterState_t *pState, lua_CFunction fn,
                     eTHLuaMetatable eMetatable1, ...);

#define luaT_class(typnam, new_fn, name, mt) { \
    const char * sCurrentClassName = name; \
    int iCurrentClassMT = pState->aiMetatables[mt]; \
    lua_settop(pState->L, pState->iTop); \
    /* Make metatable the environment for registered functions */ \
    lua_pushvalue(pState->L, iCurrentClassMT); \
    lua_replace(pState->L, LUA_ENVIRONINDEX); \
    /* Set the __gc metamethod to C++ destructor */ \
    lua_pushcclosure(pState->L, luaT_stdgc<typnam, LUA_ENVIRONINDEX>, 0); \
    lua_setfield(pState->L, iCurrentClassMT, "__gc"); \
    /* Set the depersist size */ \
    lua_pushinteger(pState->L, sizeof(typnam)); \
    lua_setfield(pState->L, iCurrentClassMT, "__depersist_size"); \
    /* Create the methods table; call it -> new instance */ \
    luaT_pushcclosuretable(pState->L, new_fn, 0); \
    /* Set __class_name on the methods metatable */ \
    lua_getmetatable(pState->L, -1); \
    lua_pushstring(pState->L, sCurrentClassName); \
    lua_setfield(pState->L, -2, "__class_name"); \
    lua_pop(pState->L, 1); \
    /* Set __index to the methods table */ \
    lua_pushvalue(pState->L, -1); \
    lua_setfield(pState->L, iCurrentClassMT, "__index")

#define luaT_endclass() \
    lua_setfield(pState->L, pState->iMainTable, sCurrentClassName); }

#define luaT_setmetamethod(fn, name, ...) \
    luaT_setclosure(pState, fn, ## __VA_ARGS__, MT_Count); \
    lua_setfield(pState->L, iCurrentClassMT, "__" name)

#define luaT_setfunction(fn, name, ...) \
    luaT_setclosure(pState, fn, ## __VA_ARGS__, MT_Count); \
    lua_setfield(pState->L, -2, name)

#endif