#pragma once
#include <lua.h>
#include <regex>
#include <string>

namespace api
{
	namespace Environment
	{
		namespace Regex
		{
			class RegexObject {
			public:
				RegexObject(const std::string& pattern);
				bool IsValid() const;
				std::string GetErrorMessage() const;
				std::vector<std::string> Match(const std::string& str) const;
				std::string Replace(const std::string& str, const std::string& replacement) const;

			private:
				std::string m_pattern;
				std::regex m_regex;
				bool m_valid;
				std::string m_errorMsg;
			};

			void Register(lua_State* L);
		}
	}
} 
