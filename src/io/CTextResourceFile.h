#pragma once

#include <string>
#include <unordered_map>

/**
* \brief A text resource file (trf) stores texts associated with ini-like tags.
* Example:
* [foo]
* abcd efgh ijkl
* mno pqr stu vwx.
* [bar]
* abc1 efg2 ijk3
* mno4 pqr5 stu6.
*
* Stores the corresponding texts associated with "foo" and "bar".
*/
class CTextResourceFile
{
public:
	bool load(const std::string& file);
	bool save(const std::string& file) const;

	/**
	* \brief Writes string associated with groupt to text.
	* \return True if group exists and text is non-empty and false if not.
	*/
	bool getText(const std::string& group, std::string& text) const;

	/**
	* \brief Sets text of group to specified value.
	* Creates new group, if the group does not exist and overrides old value otherise.
	*/
	void setText(const std::string& group, const std::string& text);

private:
	std::unordered_map<std::string, std::string> m_textMap;
};