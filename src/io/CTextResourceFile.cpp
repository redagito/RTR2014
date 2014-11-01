#include "CTextResourceFile.h"

#include <fstream>
#include <sstream>

#include "debug/Log.h"

bool CTextResourceFile::load(const std::string& file)
{
	m_textMap.clear();
	std::ifstream ifs(file);
	if (!ifs.is_open())
	{
		return false;
	}

	std::string line;
	std::string group;
	std::stringstream text;
	while (std::getline(ifs, line))
	{
		if (line.empty())
		{
			if (group.empty())
			{
				// Empty line
				continue;
			}
			else
			{
				text << std::endl;
			}
		}
		else if (line.at(0) == '[')  // Group
		{
			unsigned int pos = line.find(']');
			if (pos == std::string::npos)
			{
				m_textMap.clear();
				ifs.close();
				return false;
			}
			// Write previous data
			if (!group.empty())
			{
				m_textMap[group] = text.str();
			}

			// Set current group
			group = line.substr(1, pos - 1);
			text.clear();
		}
		else  // Text line
		{
			text << line << std::endl;
		}
	}
	ifs.close();

	// Write final data
	if (!group.empty())
	{
		m_textMap[group] = text.str();
	}

	return true;
}

bool CTextResourceFile::save(const std::string& file) const
{
	std::ofstream ofs(file);
	if (!ofs.is_open())
	{
		return false;
	}

	for (const auto& entry : m_textMap)
	{
		ofs << "[" << entry.first << "]" << std::endl;
		ofs << entry.second << std::endl;
	}
	ofs.close();
	return true;
}

bool CTextResourceFile::getText(const std::string& group, std::string& text) const
{
	const auto& entry = m_textMap.find(group);
	if (entry == m_textMap.end())
	{
		return false;
	}

	text = entry->second;
	return false;
}

void CTextResourceFile::setText(const std::string& group, const std::string& text)
{
	m_textMap[group] = text;
}