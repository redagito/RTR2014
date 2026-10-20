#pragma once

#include <map>
#include <string>

/**
 * \brief Ini file class.
 * Reads and writes ini file data.
 * Ini files consist of comments, groups and key/value pairs.
 * Example:
 * # <-- This is a comment
 * [group] <-- This is a group
 * key=value <-- This is a key/value pair
 */
class IniFile
{
   public:
    /**
     * \brief Loads file data.
     */
    [[nodiscard]] bool load(const std::string& file);

    /**
     * \brief Saves file data.
     */
    bool save(const std::string& file);

    /**
     * \brief Sets value for group and key.
     */
    void setValue(const std::string& group, const std::string& key, const std::string& value);
    void setValue(const std::string& group, const std::string& key, int value);
    void setValue(const std::string& group, const std::string& key, float value);

    /**
     * \brief Returns value for group and key.
     * Returned by value: the default argument may be a temporary.
     */
    std::string getValue(const std::string& group, const std::string& key,
                         const std::string& defaultValue) const;
    int getValue(const std::string& group, const std::string& key, int defaultValue) const;
    float getValue(const std::string& group, const std::string& key, float defaultValue) const;

    /**
     * \brief Returns whether the group exists or not.
     */
    [[nodiscard]] bool hasGroup(const std::string& group) const;

    /**
     * \brief Returns whether a key in the specified group exist.
     */
    [[nodiscard]] bool hasKey(const std::string& group, const std::string& key) const;

   private:
    std::map<std::string, std::map<std::string, std::string>> d_entries; /**< Ini file entries. */
};
