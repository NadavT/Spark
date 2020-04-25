#pragma once

namespace Spark
{
	class Time
	{
	public:
		Time(float time)
			: m_time(time)
		{

		}

		float GetSeconds() const { return m_time; }
		float GetMilliSeconds() const { return m_time * 1000.0f; }
	private:
		float m_time;
	};

	Time getCurrentTime();
}