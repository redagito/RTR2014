#include "io/IniFile.h"

#include <spdlog/spdlog.h>

#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>

namespace
{
std::string trim(const std::string& text)
{
    const size_t first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
    {
        return "";
    }
    const size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}
} // namespace

bool IniFile::load(const std::string& file)
{
    d_entries.clear();

    std::ifstream ifs(file, std::ios::binary);
    if (!ifs.is_open())
    {
        return false;
    }
    std::string line;
    std::string group;
    while (std::getline(ifs, line))
    {
        line = trim(line);
        if (line.empty())
        {
            // Empty line
            continue;
        }
        else if (line.at(0) == '#')  // Comment
        {
            // Ignore
            continue;
        }
        else if (line.at(0) == '[')  // Group
        {
            std::string::size_type pos = line.find(']');
            if (pos == std::string::npos)
            {
                d_entries.clear();
                return false;
            }
            // Set current group
            group = trim(line.substr(1, pos - 1));
        }
        else  // Key/value pair
        {
            std::string::size_type pos = line.find('=');
            if (pos == std::string::npos)
            {
                d_entries.clear();
                return false;
            }
            d_entries[group][trim(line.substr(0, pos))] = trim(line.substr(pos + 1));
        }
    }
    return true;
}

bool IniFile::save(const std::string& file)
{
    // Ini file
    std::ofstream ofs(file);
    if (!ofs.is_open())
    {
        return false;
    }

    // Write groups
    for (const auto& group : d_entries)
    {
        ofs << "[" << group.first << "]" << std::endl;
        // Write entries
        for (const auto& entry : group.second)
        {
            ofs << entry.first << "=" << entry.second << std::endl;
        }
        ofs << std::endl;
    }
    // The write can fail after the file was opened, a full disk for example.
    return ofs.good();
}

void IniFile::setValue(const std::string& group, const std::string& key, const std::string& value)
{
    d_entries[group][key] = value;
    return;
}

void IniFile::setValue(const std::string& group, const std::string& key, int value)
{
    std::stringstream ss;
    ss << value;
    setValue(group, key, ss.str());
}

void IniFile::setValue(const std::string& group, const std::string& key, float value)
{
    std::stringstream ss;
    // The full precision is needed, the value has to survive a round trip.
    ss << std::setprecision(std::numeric_limits<float>::max_digits10) << value;
    setValue(group, key, ss.str());
}

std::string IniFile::getValue(const std::string& group, const std::string& key,
                              const std::string& defaultValue) const
{
    const auto groupEntry = d_entries.find(group);
    if (groupEntry == d_entries.end())
    {
        return defaultValue;
    }
    const auto keyEntry = groupEntry->second.find(key);
    if (keyEntry == groupEntry->second.end())
    {
        return defaultValue;
    }
    return keyEntry->second;
}

int IniFile::getValue(const std::string& group, const std::string& key, int defaultValue) const
{
    const auto groupEntry = d_entries.find(group);
    if (groupEntry == d_entries.end())
    {
        return defaultValue;
    }
    const auto keyEntry = groupEntry->second.find(key);
    if (keyEntry == groupEntry->second.end())
    {
        return defaultValue;
    }
    try
    {
        size_t pos = 0;
        int value = std::stoi(keyEntry->second, &pos);
        if (pos != keyEntry->second.size())
        {
            spdlog::warn("Trailing characters in ini value for [{}] {}.", group.c_str(),
                         key.c_str());
        }
        return value;
    }
    catch (const std::exception&)
    {
        spdlog::warn("Failed to parse int ini value for [{}] {}.", group.c_str(), key.c_str());
    }
    return defaultValue;
}

float IniFile::getValue(const std::string& group, const std::string& key, float defaultValue) const
{
    const auto groupEntry = d_entries.find(group);
    if (groupEntry == d_entries.end())
    {
        return defaultValue;
    }
    const auto keyEntry = groupEntry->second.find(key);
    if (keyEntry == groupEntry->second.end())
    {
        return defaultValue;
    }
    try
    {
        size_t pos = 0;
        float value = std::stof(keyEntry->second, &pos);
        if (pos != keyEntry->second.size())
        {
            spdlog::warn("Trailing characters in ini value for [{}] {}.", group.c_str(),
                         key.c_str());
        }
        return value;
    }
    catch (const std::exception&)
    {
        spdlog::warn("Failed to parse float ini value for [{}] {}.", group.c_str(),
                     key.c_str());
    }
    return defaultValue;
}

bool IniFile::hasGroup(const std::string& group) const { return d_entries.count(group) == 1; }

bool IniFile::hasKey(const std::string& group, const std::string& key) const
{
    return hasGroup(group) && d_entries.at(group).count(key) == 1;
}
