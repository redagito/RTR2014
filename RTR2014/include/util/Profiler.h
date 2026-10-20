#pragma once

#include <chrono>
#include <mutex>
#include <string>
#include <vector>

/**
* \brief Collects timing data for named sections.
*
* Sections are measured with a ScopedTimer. Every section keeps the number of
* times it was entered, the total time spent in it and the fastest and slowest
* single visit. Timings may be recorded from any thread.
*/
class Profiler
{
   public:
    /**
    * \brief Records the time spent in a section.
    */
    void add(const std::string& name, double milliseconds);

    /**
    * \brief Writes the collected timings to a file.
    *
    * Returns false if the file could not be written.
    */
    bool writeReport(const std::string& file) const;

   private:
    struct Section
    {
        std::string name;
        unsigned long long count = 0;
        double total = 0.0;
        double min = 0.0;
        double max = 0.0;
    };

    mutable std::mutex m_mutex;
    std::vector<Section> m_sections;
};

/**
* \brief Times a section for the lifetime of the object.
*/
class ScopedTimer
{
   public:
    ScopedTimer(Profiler& profiler, const std::string& name);
    ~ScopedTimer();

   private:
    Profiler& m_profiler;
    std::string m_name;
    std::chrono::steady_clock::time_point m_start;
};

/**
* \brief Returns the process wide profiler.
*/
Profiler& profiler();