extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <iostream>
#include <sstream>

#include "test1.h"
#include "test2.h"


int error(lua_State* state) {
    std::cout << "Error, Top : " << lua_gettop(state) << std::endl;
    std::cout << lua_tostring(state, 1) << std::endl;
    return 0;
}



int main(int /*argc*/, char **/*argv*/)
{
    lua_State* state = luaL_newstate();

    // pour print
    luaL_openlibs(state);

    // Push de la méthode de gestion des erreurs et recup de l'index
    lua_pushcfunction(state, error);
    int errorIdx = lua_gettop(state);

    // Test1 don't create lib and class paradigm
    test1(state, errorIdx);

    // Test 2 is more complicated :
    //    + OOP Paradigm
    //    + Lua5.3 compatible
    //    + Module
    test2(state, errorIdx);

    lua_close(state);
}
