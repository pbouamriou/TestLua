#include <test2.h>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <iostream>
#include <string>
#include <sstream>

#define TIMER "Timer"

// Simulate a timer class
// The goal of this class is to test Lua Integration
class Timer {
public:
    Timer() : _duration(1000), _isRunning(false) {
    }
    void setDuration(int ms) {
        _duration = ms;
    }
    bool start() {
        if(_isRunning) {
            return false;
        }
        _isRunning = true;
        std::cout << "Timer started, duration : " << _duration << std::endl;
        return true;
    }
    bool stop(){
        if(!_isRunning) {
            return false;
        }
        _isRunning = false;
        std::cout << "Timer stopped" << std::endl;
        return true;
    }

    std::string toString() {
        std::stringstream stream;
        stream << "Timer(" << (_isRunning ? "Running" : "Not Running") << ", " << _duration << ")";
        return stream.str();
    }
private:
    int _duration;
    bool _isRunning;
};


// Meta methods
int timer_tostring(lua_State* state) {
    Timer** ud = (Timer**)lua_touserdata(state, 1);
    Timer* timer = *ud;
    lua_pushstring(state, timer->toString().c_str());
    return 1;
}


// Methods
int timer_start(lua_State* state) {
    Timer** ud = (Timer**)lua_touserdata(state, 1);
    Timer* timer = *ud;
    timer->start();
    return 0;
}
int timer_setduration(lua_State* state) {
    Timer** ud = (Timer**)lua_touserdata(state, 1);
    Timer* timer = *ud;
    if( lua_isinteger(state, 2) ) {
        int duration = lua_tointeger(state, 2);
        timer->setDuration(duration);
    } else {
        luaL_error(state, "set_duration need integer parameter");
    }
    return 0;
}

int timer_stop(lua_State* state) {
    Timer** ud = (Timer**)lua_touserdata(state, 1);
    Timer* timer = *ud;
    timer->stop();
    return 0;
}

int timer_new(lua_State* state) {
    Timer * timer = new Timer;
    Timer** ud = (Timer**)lua_newuserdata(state, sizeof(Timer*));
    *ud = timer;

    luaL_getmetatable(state, TIMER);
    lua_setmetatable(state, -2);
    return 1;
}

int timer_gc(lua_State* state) {
    Timer** ud = (Timer**)lua_touserdata(state, 1);
    delete *ud;
    std::cout << "GC called" << std::endl;
    return 0;
}

static const luaL_Reg timer_meta[] {
    { "__tostring", timer_tostring } ,
    { "__gc", timer_gc } ,
    { nullptr, nullptr}
};

static const luaL_Reg timer_methods[] {
    { "start", timer_start },
    { "stop", timer_stop},
    { "set_duration", timer_setduration},
    { nullptr, nullptr }
};

static const luaL_Reg timer_staticmethods[] {
    { "new", timer_new},
    { nullptr, nullptr }
};

int luaopen_timer(lua_State* state) {
    // Create module Timer and add timer_methods to it
    luaL_newlib(state, timer_staticmethods);

    // Create meta table for Timer object
    // This meta table is stored in the registry
    luaL_newmetatable(state, TIMER);

    // Add timer_meta functions to the metatable
    luaL_setfuncs(state, timer_meta, 0);

    // Associate methods to metatable trough a new table linked to __index
    luaL_newlib(state, timer_methods);
    lua_setfield(state, -2, "__index");

    // Remove metatable from stack and keep library table
    lua_pop(state, 1);

    return 1;
}

void test2(lua_State* state, int errorIdx)
{
    std::cout << "***************************************************************************" << std::endl;
    std::cout << "Test 2" << std::endl;

    luaL_requiref(state, TIMER, luaopen_timer, 1);
    lua_pop(state, 1); // remove module

    luaL_loadstring(state, "timer = Timer.new()\n"
                           "print(timer)\n"
                           "timer:set_duration(100)\n"
                           "print(timer)\n"
                           "timer:start()\n"
                           "print(timer)\n"
                           "timer:stop()\n"
                           "timer=nil") || lua_pcall(state, 0, LUA_MULTRET, errorIdx);

    std::cout << "***************************************************************************" << std::endl;
}
