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

    Time(double time)
        : m_time(time)
    {
    }

    inline double GetSeconds() const
    {
        return m_time;
    }
    inline double GetMilliSeconds() const
    {
        return m_time * 1000.0f;
    }

    Time operator+(Time const &other)
    {
        return Time(m_time + other.m_time);
    }
    Time operator-(Time const &other)
    {
        return Time(m_time - other.m_time);
    }

  private:
    double m_time;
};

Time getCurrentTime();
void sleep(const Time &time);
} // namespace Spark