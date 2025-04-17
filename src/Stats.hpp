#pragma once

#include <chrono>
#include <limits>
#include <iostream>
#include <iomanip>

class Stats {
  public:
    Stats() : m_measureTimeMin(std::numeric_limits<double>::max()),
              m_measureTimeMax(0.0),
              m_measureTimeAvg(0.0),
              m_measurerTotalTime(0.0),
              m_measureCount(0)
    {

    }

    void StartTimer()
    {
        m_startTime = std::chrono::high_resolution_clock::now();
    }
    void StopTimer()
    {
        m_endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(m_endTime - m_startTime);
        double microSecs = duration.count() / 1000.0;
        m_measurerTotalTime += microSecs;
        if (microSecs > m_measureTimeMax)
            m_measureTimeMax = microSecs;
        else if (microSecs < m_measureTimeMin)
            m_measureTimeMin = microSecs;
        m_measureCount++;
        m_measureTimeAvg = (m_measureTimeAvg * (m_measureCount-1) + microSecs) / m_measureCount;
    }
    double GetTotalTimeMsecs()
    {
       return m_measurerTotalTime / 1000.0;
    }

    void DumpStats()
    {
        std::cout << "    avg=" << std::fixed << std::setprecision(3) << m_measureTimeAvg << " usecs" << std::endl;
        std::cout << "    max=" << std::fixed << std::setprecision(3) << m_measureTimeMax << " usecs" << std::endl;
        std::cout << "    min=" << std::fixed << std::setprecision(3) << m_measureTimeMin << " usecs" << std::endl;
    }

  private:
    double  m_measureTimeMin;
    double  m_measureTimeMax;
    double  m_measureTimeAvg;
    double  m_measurerTotalTime;
    size_t  m_measureCount;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_endTime;
};
