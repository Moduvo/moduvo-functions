#include "Stopwatch.hpp"
#include "RBX.hpp"
#include "../../Dependencies/Luau/VM/src/lgc.h"
#include "../../Dependencies/Luau/VM/src/lapi.h"
#include <lualib.h>
#include <chrono>
#include <stdio.h>

#define RegisterFunction(L, Func, Name) lua_pushcclosure(L, Func, Name, 0); \
lua_setglobal(L, Name);

#define RegisterMember(L, Func, Name) lua_pushcclosure(L, Func, Name, 0); \
lua_setfield(L, -2, Name);

namespace api {
    namespace Environment {
        namespace Stopwatch {
            
            StopwatchObject::StopwatchObject() 
                : m_isRunning(false), m_elapsedSeconds(0.0)
            {
                m_startTime = std::chrono::high_resolution_clock::now();
                m_stopTime = m_startTime;
            }

            void StopwatchObject::Start()
            {
                if (!m_isRunning) {
                    m_isRunning = true;
                    m_startTime = std::chrono::high_resolution_clock::now();
                }
            }

            void StopwatchObject::Stop()
            {
                if (m_isRunning) {
                    m_stopTime = std::chrono::high_resolution_clock::now();
                    m_elapsedSeconds += std::chrono::duration<double>(m_stopTime - m_startTime).count();
                    m_isRunning = false;
                }
            }

            void StopwatchObject::Reset()
            {
                m_isRunning = false;
                m_elapsedSeconds = 0.0;
                m_startTime = std::chrono::high_resolution_clock::now();
                m_stopTime = m_startTime;
            }

            double StopwatchObject::ElapsedTime() const
            {
                if (m_isRunning) {
                    auto now = std::chrono::high_resolution_clock::now();
                    return m_elapsedSeconds + std::chrono::duration<double>(now - m_startTime).count();
                } else {
                    return m_elapsedSeconds;
                }
            }

            static const char* STOPWATCH_MT = "Stopwatch_MT";

            static StopwatchObject* GetStopwatchObject(lua_State* L, int idx)
            {
                return *(StopwatchObject**)luaL_checkudata(L, idx, STOPWATCH_MT);
            }

            static int StopwatchGC(lua_State* L)
            {
                StopwatchObject** udata = (StopwatchObject**)lua_touserdata(L, 1);
                if (udata && *udata) {
                    delete *udata;
                    *udata = nullptr;
                }
                return 0;
            }

            // Lua function: Stopwatch.new()
            static int StopwatchNew(lua_State* L)
            {
                StopwatchObject** udata = (StopwatchObject**)lua_newuserdata(L, sizeof(StopwatchObject*));
                *udata = new StopwatchObject();
                
                luaL_getmetatable(L, STOPWATCH_MT);
                lua_setmetatable(L, -2);
                
                return 1;
            }

            // Method: stopwatch:Start()
            static int StopwatchStart(lua_State* L)
            {
                StopwatchObject* stopwatch = GetStopwatchObject(L, 1);
                stopwatch->Start();
                return 0;
            }

            // Method: stopwatch:Stop()
            static int StopwatchStop(lua_State* L)
            {
                StopwatchObject* stopwatch = GetStopwatchObject(L, 1);
                stopwatch->Stop();
                return 0;
            }

            // Method: stopwatch:Reset()
            static int StopwatchReset(lua_State* L)
            {
                StopwatchObject* stopwatch = GetStopwatchObject(L, 1);
                stopwatch->Reset();
                return 0;
            }

            // Method: stopwatch:ElapsedTime()
            static int StopwatchElapsedTime(lua_State* L)
            {
                StopwatchObject* stopwatch = GetStopwatchObject(L, 1);
                double elapsedTime = stopwatch->ElapsedTime();
                
                char buffer[64];
                sprintf(buffer, "%.6f", elapsedTime);
                
                lua_pushstring(L, buffer);
                
                return 1;
            }

            void Register(lua_State* L)
            {
                luaL_newmetatable(L, STOPWATCH_MT);
                
                RegisterMember(L, StopwatchGC, "__gc");
                
                lua_createtable(L, 0, 4);
                
                RegisterMember(L, StopwatchStart, "Start");
                RegisterMember(L, StopwatchStop, "Stop");
                RegisterMember(L, StopwatchReset, "Reset");
                RegisterMember(L, StopwatchElapsedTime, "ElapsedTime");
                
                lua_setfield(L, -2, "__index");
                
                lua_pop(L, 1);
                
                lua_newtable(L);
                
                RegisterMember(L, StopwatchNew, "new");
                
                lua_setglobal(L, "Stopwatch");
            }
        }
    }
}
