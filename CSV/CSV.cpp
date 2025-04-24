#include "CSV.hpp"
#include "RBX.hpp"
#include "../../Dependencies/Luau/VM/src/lgc.h"
#include "../../Dependencies/Luau/VM/src/lapi.h"
#include <lualib.h>
#include <string>
#include <vector>
#include <sstream>

#define RegisterFunction(L, Func, Name) lua_pushcclosure(L, Func, Name, 0); \
lua_setglobal(L, Name);

#define RegisterMember(L, Func, Name) lua_pushcclosure(L, Func, Name, 0); \
lua_setfield(L, -2, Name);

namespace api {
    namespace Environment {
        namespace CSV {

            static std::vector<std::string> parseLine(const std::string& line, char delimiter) {
                std::vector<std::string> result;
                std::stringstream ss(line);
                std::string item;
                bool inQuotes = false;
                std::string field;
                
                for (size_t i = 0; i < line.length(); ++i) {
                    char c = line[i];
                    
                    if (c == '"') {
                        inQuotes = !inQuotes;
                    } else if (c == delimiter && !inQuotes) {
                        result.push_back(field);
                        field.clear();
                    } else {
                        field += c;
                    }
                }
                
                result.push_back(field);
                return result;
            }

            // Function: csv.parse(csvString, delimiter)
            static int CSV_Parse(lua_State* L) {
                if (!lua_isstring(L, 1)) {
                    lua_pushstring(L, "Expected string for CSV data");
                    lua_error(L);
                    return 0;
                }
                
                const char* csvString = lua_tostring(L, 1);
                
                char delimiter = ',';
                if (lua_isstring(L, 2) && lua_strlen(L, 2) > 0) {
                    delimiter = lua_tostring(L, 2)[0];
                }

                lua_newtable(L);
                
                std::string csv(csvString);
                std::string line;
                std::istringstream iss(csv);
                int rowIndex = 1;
                
                while (std::getline(iss, line)) {
                    if (line.empty()) continue;
                    
                    std::vector<std::string> fields = parseLine(line, delimiter);
                    
                    lua_createtable(L, (int)fields.size(), 0);
                    
                    for (size_t i = 0; i < fields.size(); ++i) {
                        lua_pushstring(L, fields[i].c_str());
                        lua_rawseti(L, -2, (int)(i + 1));
                    }
                    
                    lua_rawseti(L, -2, rowIndex++);
                }
                
                return 1;
            }

            // Function: csv.stringify(table, delimiter)
            static int CSV_Stringify(lua_State* L) {
                if (!lua_istable(L, 1)) {
                    lua_pushstring(L, "Expected table for CSV data");
                    lua_error(L);
                    return 0;
                }
                
                char delimiter = ',';
                if (lua_isstring(L, 2) && lua_strlen(L, 2) > 0) {
                    delimiter = lua_tostring(L, 2)[0];
                }
                
                std::stringstream result;
                int numRows = (int)lua_objlen(L, 1);
                
                for (int i = 1; i <= numRows; ++i) {
                    lua_rawgeti(L, 1, i);
                    
                    if (!lua_istable(L, -1)) {
                        lua_pop(L, 1);
                        continue;
                    }
                    
                    int numCols = (int)lua_objlen(L, -1);
                    
                    for (int j = 1; j <= numCols; ++j) {
                        lua_rawgeti(L, -1, j);
                        
                        const char* fieldValue = "";
                        if (!lua_isnil(L, -1)) {
                            fieldValue = lua_tostring(L, -1);
                        }
                        
                        std::string field(fieldValue ? fieldValue : "");
                        bool needQuotes = field.find(delimiter) != std::string::npos || 
                                         field.find('"') != std::string::npos || 
                                         field.find('\n') != std::string::npos;
                        
                        if (needQuotes) {
                            size_t pos = 0;
                            while ((pos = field.find('"', pos)) != std::string::npos) {
                                field.replace(pos, 1, "\"\"");
                                pos += 2;
                            }
                            
                            result << "\"" << field << "\"";
                        } else {
                            result << field;
                        }
                        
                        if (j < numCols) {
                            result << delimiter;
                        }
                        
                        lua_pop(L, 1);
                    }
                    
                    if (i < numRows) {
                        result << "\n";
                    }
                    
                    lua_pop(L, 1);
                }
                
                lua_pushstring(L, result.str().c_str());
                return 1;
            }

            // Function: csv.getField(csvTable, row, col)
            static int CSV_GetField(lua_State* L) {
                if (!lua_istable(L, 1)) {
                    lua_pushstring(L, "Expected table for CSV data");
                    lua_error(L);
                    return 0;
                }
                
                int row = (int)luaL_checknumber(L, 2);
                int col = (int)luaL_checknumber(L, 3);
                
                lua_rawgeti(L, 1, row);
                if (!lua_istable(L, -1)) {
                    lua_pushnil(L);
                    return 1;
                }
                
                lua_rawgeti(L, -1, col);
                
                return 1;
            }

            // Function: csv.getRow(csvTable, row)
            static int CSV_GetRow(lua_State* L) {
                if (!lua_istable(L, 1)) {
                    lua_pushstring(L, "Expected table for CSV data");
                    lua_error(L);
                    return 0;
                }
                
                int row = (int)luaL_checknumber(L, 2);
                
                lua_rawgeti(L, 1, row);
                
                return 1;
            }

            void Register(lua_State* L) {
                lua_newtable(L);
                
                RegisterMember(L, CSV_Parse, "parse");
                RegisterMember(L, CSV_Stringify, "stringify");
                RegisterMember(L, CSV_GetField, "getField");
                RegisterMember(L, CSV_GetRow, "getRow");
                
                lua_setglobal(L, "csv");
            }
        }
    }
}
