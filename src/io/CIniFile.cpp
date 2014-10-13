#include "CIniFile.h"

#include <fstream>
#include <sstream>

bool CIniFile::load(const std::string& file)
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
            unsigned int pos = line.find(']');
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
            unsigned int pos = line.find('=');
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

bool CIniFile::save(const std::string& file)
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

void CIniFile::setValue(const std::string& group, const std::string& key, const std::string& value)
{
    d_entries[group][key] = value;
    return;
}

void CIniFile::setValue(const std::string& group, const std::string& key, int value)
{
    std::stringstream ss;
    ss << value;
    setValue(group, key, ss.str());
}

void CIniFile::setValue(const std::string& group, const std::string& key, float value)
{
    std::stringstream ss;
    ss << value;
    setValue(group, key, ss.str());
}

const std::string& CIniFile::getValue(const std::string& group, const std::string& key,
                                      const std::string& defaultValue) const
{
    if (hasKey(group, key))
    {
        return d_entries.at(group).at(key);
    }
    return defaultValue;
}

int CIniFile::getValue(const std::string& group, const std::string& key, int defaultValue) const
{
    if (hasKey(group, key))
    {
        try
        {
            return std::stoi(d_entries.at(group).at(key));
        }
        catch (const std::exception&)
        {
            // Conversion error
        }
    }
    return defaultValue;
}

float CIniFile::getValue(const std::string& group, const std::string& key, float defaultValue) const
{
    if (hasKey(group, key))
    {
        try
        {
            return std::stof(d_entries.at(group).at(key));
        }
        catch (const std::exception&)
        {
            // Conversion error
        }
    }
    return defaultValue;
}

bool CIniFile::hasGroup(const std::string& group) const { return d_entries.count(group) == 1; }

bool CIniFile::hasKey(const std::string& group, const std::string& key) const
{
    return hasGroup(group) && d_entries.at(group).count(key) == 1;
}
