#include "io/IniFile.h"

#include <spdlog/spdlog.h>

#include <fstream>
#include <sstream>

bool IniFile::load(const std::string& file)
{
    d_entries.clear();

    std::ifstream ifs(file);
    if (!ifs.is_open())
    {
        return false;
    }
    std::string line;
    std::string group;
    while (std::getline(ifs, line))
    {
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
                ifs.close();
                return false;
            }
            // Set current group
            group = line.substr(1, pos - 1);
        }
        else  // Key/value pair
        {
            std::string::size_type pos = line.find('=');
            if (pos == std::string::npos)
            {
                d_entries.clear();
                ifs.close();
                return false;
            }
            d_entries[group][line.substr(0, pos)] = line.substr(pos + 1);
        }
    }
    ifs.close();
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
    return true;
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
    ss << value;
    setValue(group, key, ss.str());
}

std::string IniFile::getValue(const std::string& group, const std::string& key,
                              const std::string& defaultValue) const
{
    if (hasKey(group, key))
    {
        return d_entries.at(group).at(key);
    }
    return defaultValue;
}

int IniFile::getValue(const std::string& group, const std::string& key, int defaultValue) const
{
    if (hasKey(group, key))
    {
        try
        {
            size_t pos = 0;
            int value = std::stoi(d_entries.at(group).at(key), &pos);
            if (pos != d_entries.at(group).at(key).size())
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
    }
    return defaultValue;
}

float IniFile::getValue(const std::string& group, const std::string& key, float defaultValue) const
{
    if (hasKey(group, key))
    {
        try
        {
            size_t pos = 0;
            float value = std::stof(d_entries.at(group).at(key), &pos);
            if (pos != d_entries.at(group).at(key).size())
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
    }
    return defaultValue;
}

bool IniFile::hasGroup(const std::string& group) const { return d_entries.count(group) == 1; }

bool IniFile::hasKey(const std::string& group, const std::string& key) const
{
    return hasGroup(group) && d_entries.at(group).count(key) == 1;
}
