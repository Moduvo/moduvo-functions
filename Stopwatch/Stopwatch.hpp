#pragma once
#include <lua.h>
#include <chrono>

namespace api
{
	namespace Environment
	{
		namespace Stopwatch
		{
			class StopwatchObject {
			public:
				StopwatchObject();
				void Start();
				void Stop();
				void Reset();
				double ElapsedTime() const;

			private:
				std::chrono::high_resolution_clock::time_point m_startTime;
				std::chrono::high_resolution_clock::time_point m_stopTime;
				bool m_isRunning;
				double m_elapsedSeconds;
			};

			void Register(lua_State* L);
		}
	}
} 
