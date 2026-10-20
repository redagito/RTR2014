#include "util/Profiler.h"

#include <algorithm>
#include <fstream>
#include <iomanip>

void Profiler::add(const std::string& name, double milliseconds)
{
    const std::lock_guard<std::mutex> lock(m_mutex);

    for (Section& section : m_sections)
    {
        if (section.name == name)
        {
            section.count++;
            section.total += milliseconds;
            section.min = std::min(section.min, milliseconds);
            section.max = std::max(section.max, milliseconds);
            return;
        }
    }

    Section section;
    section.name = name;
    section.count = 1;
    section.total = milliseconds;
    section.min = milliseconds;
    section.max = milliseconds;
    m_sections.push_back(section);
}

bool Profiler::writeReport(const std::string& file) const
{
    std::ofstream out(file);
    if (!out.is_open())
    {
        return false;
    }

    const std::lock_guard<std::mutex> lock(m_mutex);

    out << std::left << std::setw(34) << "Section" << std::right << std::setw(9) << "Count"
        << std::setw(14) << "Total(ms)" << std::setw(12) << "Avg(ms)" << std::setw(12) << "Min(ms)"
        << std::setw(12) << "Max(ms)" << "\n";
    out << std::string(93, '-') << "\n";

    for (const Section& section : m_sections)
    {
        const double average = section.count > 0 ? section.total / section.count : 0.0;
        out << std::left << std::setw(34) << section.name << std::right << std::setw(9)
            << section.count << std::fixed << std::setprecision(2) << std::setw(14) << section.total
            << std::setw(12) << average << std::setw(12) << section.min << std::setw(12)
            << section.max << "\n";
    }
    return out.good();
}

ScopedTimer::ScopedTimer(Profiler& profiler, const std::string& name)
    : m_profiler(profiler), m_name(name), m_start(std::chrono::steady_clock::now())
{
}

ScopedTimer::~ScopedTimer()
{
    const double elapsed =
        std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - m_start)
            .count();
    m_profiler.add(m_name, elapsed);
}

Profiler& profiler()
{
    static Profiler instance;
    return instance;
}