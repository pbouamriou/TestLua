#include <test1.h>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <iostream>
#include <string>
#include <sstream>

struct Test {
    int x;
    int y;

    static int createTest(lua_State* state) {
        Test * test = new Test;
        Test** ud = (Test**)lua_newuserdata(state, sizeof(Test*));
        *ud = test;
        if( lua_gettop(state) >= 3) {
            test->x = lua_tointeger(state, 1);
            test->y = lua_tointeger(state, 2);
        } else {
            test->x = 0;
            test->y = 0;
        }

        //luaL_getmetatable(state, "Test");
        lua_pushstring(state, "Test");
        lua_gettable(state, LUA_REGISTRYINDEX);

        lua_setmetatable(state, -2);

        std::cout << "Top (CreateTest) " << lua_gettop(state) << std::endl;

        return 1;
    }

    static int to_string(lua_State* state) {
        Test** ud = (Test**)lua_touserdata(state, 1);
        std::stringstream stream;
        stream << (*ud)->x << " : " << (*ud)->y ;
        lua_pushstring(state, stream.str().c_str());
        return 1;
    }

    static int gc(lua_State* state) {
        Test** ud = (Test**)lua_touserdata(state, 1);
        delete *ud;
        return 0;
    }

    static int index(lua_State* state) {
        Test** ud = (Test**)lua_touserdata(state, 1);
        std::string name = lua_tostring(state, 2);
        if( name == "x") {
            lua_pushinteger(state, (*ud)->x);
        } else if( name == "y" ) {
            lua_pushinteger(state, (*ud)->y);
        } else {
            lua_pushnil(state);
        }
        return 1;
    }

};

int mafct(lua_State* state) {
    std::cout << "Top : " << lua_gettop(state) << std::endl;
    lua_pushinteger(state, 10);
    lua_pushinteger(state, 20);
    lua_pushinteger(state, 30);
    return 3;
}

void test1(lua_State* state, int errorIdx) {
    std::cout << "***************************************************************************" << std::endl;
    std::cout << "Test 1" << std::endl;
    // Creation de la métatable Test et ajout des méthodes __gc, __tostring, __index
    luaL_newmetatable(state, "Test");
    lua_pushstring(state, "__tostring");
    lua_pushcfunction(state, Test::to_string);
    lua_settable(state, -3);
    lua_pushstring(state, "__gc");
    lua_pushcfunction(state, Test::gc);
    lua_settable(state, -3);
    lua_pushstring(state, "__index");
    lua_pushcfunction(state, Test::index);
    lua_settable(state, -3);

    lua_pop(state, 1);

    // Pour tests
    lua_pushstring(state, "Test1");
    lua_pushstring(state, "Test2");
    std::cout << "Top : " << lua_gettop(state) << std::endl;
    lua_register(state, "mafct", mafct);
    lua_register(state, "createTest", Test::createTest);
    lua_pushstring(state, "Test2");
    lua_getglobal(state, "mafct");
    lua_pcall(state, 0, 2, errorIdx);
    std::cout << "Top : " << lua_gettop(state) << std::endl;
    std::cout << "Return value : " << lua_tointeger(state, -2) << std::endl;
    luaL_loadstring(state, "x = createTest(1,2)\n"
                           "print(x)\n"
                           "print(x.x + x.y)") || lua_pcall(state, 0, LUA_MULTRET, errorIdx);
    std::cout << "***************************************************************************" << std::endl;
}
