#pragma once

#include "spark/core/core.h"

namespace Spark
{
	class SPARK_API Time
	{
	public:
		Time()
			: m_time(0)
		{

		}

		Time(float time)
			: m_time(time)
		{

		}

		float GetSeconds() const { return m_time; }
		float GetMilliSeconds() const { return m_time * 1000.0f; }

		Time operator+(Time const& other) { return Time(m_time + other.m_time); }
		Time operator-(Time const& other) { return Time(m_time - other.m_time); }
	private:
		float m_time;
	};

	Time getCurrentTime();
}