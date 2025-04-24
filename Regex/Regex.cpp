#include "Regex.hpp"
#include "RBX.hpp"
#include "../../Dependencies/Luau/VM/src/lgc.h"
#include "../../Dependencies/Luau/VM/src/lapi.h"
#include <lualib.h>
#include <regex>
#include <vector>
#include <string>

#define RegisterFunction(L, Func, Name) lua_pushcclosure(L, Func, Name, 0); \
lua_setglobal(L, Name);

#define RegisterMember(L, Func, Name) lua_pushcclosure(L, Func, Name, 0); \
lua_setfield(L, -2, Name);

namespace api {
    namespace Environment {
        namespace Regex {
            
            RegexObject::RegexObject(const std::string& pattern) : m_pattern(pattern), m_valid(true), m_errorMsg("")
            {
                try {
                    m_regex = std::regex(pattern);
                }
                catch (const std::regex_error& e) {
                    m_valid = false;
                    m_errorMsg = e.what();
                }
            }

            bool RegexObject::IsValid() const
            {
                return m_valid;
            }

            std::string RegexObject::GetErrorMessage() const
            {
                return m_errorMsg;
            }

            std::vector<std::string> RegexObject::Match(const std::string& str) const
            {
                std::vector<std::string> matches;
                if (!m_valid) return matches;

                std::sregex_iterator it(str.begin(), str.end(), m_regex);
                std::sregex_iterator end;

                while (it != end) {
                    matches.push_back(it->str());
                    ++it;
                }

                return matches;
            }

            std::string RegexObject::Replace(const std::string& str, const std::string& replacement) const
            {
                if (!m_valid) return str;
                return std::regex_replace(str, m_regex, replacement, std::regex_constants::format_first_only);
            }

            static const char* REGEX_MT = "Regex_MT";

            static RegexObject* GetRegexObject(lua_State* L, int idx)
            {
                return *(RegexObject**)luaL_checkudata(L, idx, REGEX_MT);
            }

            static int RegexGC(lua_State* L)
            {
                RegexObject** udata = (RegexObject**)lua_touserdata(L, 1);
                if (udata && *udata) {
                    delete *udata;
                    *udata = nullptr;
                }
                return 0;
            }

            // Lua function: Regex.new(pattern)
            static int RegexNew(lua_State* L)
            {
                if (!lua_isstring(L, 1)) {
                    lua_pushstring(L, "Expected string for pattern");
                    lua_error(L);
                    return 0;
                }

                const char* pattern = lua_tostring(L, 1);
                
                RegexObject** udata = (RegexObject**)lua_newuserdata(L, sizeof(RegexObject*));
                *udata = new RegexObject(pattern);
                
                luaL_getmetatable(L, REGEX_MT);
                lua_setmetatable(L, -2);
                
                return 1;
            }

            // Lua function: Regex.Escape(str)
            static int RegexEscape(lua_State* L)
            {
                if (!lua_isstring(L, 1)) {
                    lua_pushstring(L, "Expected string to escape");
                    lua_error(L);
                    return 0;
                }

                const char* str = lua_tostring(L, 1);
                std::string input(str);
                std::string result;
                
                const std::string specialChars = "\\^$.*+?()[]{}|";
                
                for (char c : input) {
                    if (specialChars.find(c) != std::string::npos) {
                        result.push_back('\\');
                    }
                    result.push_back(c);
                }
                
                lua_pushstring(L, result.c_str());
                return 1;
            }

            // Method: regex:Status()
            static int RegexStatus(lua_State* L)
            {
                RegexObject* regex = GetRegexObject(L, 1);
                
                lua_pushboolean(L, regex->IsValid());
                lua_pushstring(L, regex->GetErrorMessage().c_str());
                
                return 2;
            }

            // Method: regex:Match(str)
            static int RegexMatch(lua_State* L)
            {
                RegexObject* regex = GetRegexObject(L, 1);
                
                if (!lua_isstring(L, 2)) {
                    lua_pushstring(L, "Expected string to match against");
                    lua_error(L);
                    return 0;
                }
                
                const char* str = lua_tostring(L, 2);
                std::vector<std::string> matches = regex->Match(str);
                
                lua_createtable(L, (int)matches.size(), 0);
                
                for (size_t i = 0; i < matches.size(); i++) {
                    lua_pushstring(L, matches[i].c_str());
                    lua_rawseti(L, -2, (int)(i + 1));
                }
                
                return 1;
            }

            // Method: regex:Replace(str, replacement)
            static int RegexReplace(lua_State* L)
            {
                RegexObject* regex = GetRegexObject(L, 1);
                
                if (!lua_isstring(L, 2)) {
                    lua_pushstring(L, "Expected string to modify");
                    lua_error(L);
                    return 0;
                }
                
                if (!lua_isstring(L, 3)) {
                    lua_pushstring(L, "Expected string for replacement");
                    lua_error(L);
                    return 0;
                }
                
                const char* str = lua_tostring(L, 2);
                const char* replacement = lua_tostring(L, 3);
                
                std::string result = regex->Replace(str, replacement);
                lua_pushstring(L, result.c_str());
                
                return 1;
            }

            void Register(lua_State* L)
            {
                luaL_newmetatable(L, REGEX_MT);
                
                RegisterMember(L, RegexGC, "__gc");
                
                lua_createtable(L, 0, 3);
                
                RegisterMember(L, RegexStatus, "Status");
                RegisterMember(L, RegexMatch, "Match");
                RegisterMember(L, RegexReplace, "Replace");
                
                lua_setfield(L, -2, "__index");
                
                lua_pop(L, 1);
                
                lua_newtable(L);
                
                RegisterMember(L, RegexNew, "new");
                
                RegisterMember(L, RegexEscape, "Escape");
                
                lua_setglobal(L, "Regex");
            }
        }
    }
} 
