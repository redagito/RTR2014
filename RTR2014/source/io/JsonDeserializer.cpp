#include <spdlog/spdlog.h>
#include "io/JsonDeserializer.h"

#include <json/json.h>


bool deserialize(const Json::Value& node, bool& b)
{
	if (node.empty())
	{
		spdlog::error("Node is empty.");
		return false;
	}
	if (!node.isBool())
	{
		spdlog::error("Node data type is not boolean.");
		return false;
	}
	b = node.asBool();
	return true;
}

bool deserialize(const Json::Value& node, int8_t& val)
{
	if (node.empty())
	{
		spdlog::error("Node is empty.");
		return false;
	}
	if (!node.isNumeric())
	{
		spdlog::error("Node data type is not numeric.");
		return false;
	}
	val = (int8_t)node.asInt();
	return true;
}

bool deserialize(const Json::Value& node, int16_t& val)
{
	if (node.empty())
	{
		spdlog::error("Node is empty.");
		return false;
	}
	if (!node.isNumeric())
	{
		spdlog::error("Node data type is not numeric.");
		return false;
	}
	val = (int16_t)node.asInt();
	return true;
}

bool deserialize(const Json::Value& node, int32_t& val)
{
	if (node.empty())
	{
		spdlog::error("Node is empty.");
		return false;
	}
	if (!node.isNumeric())
	{
		spdlog::error("Node data type is not numeric.");
		return false;
	}
	val = (int32_t)node.asInt();
	return true;
}

bool deserialize(const Json::Value& node, int64_t& val)
{
	if (node.empty())
	{
		spdlog::error("Node is empty.");
		return false;
	}
	if (!node.isNumeric())
	{
		spdlog::error("Node data type is not numeric.");
		return false;
	}
	val = node.asInt64();
	return true;
}

bool deserialize(const Json::Value& node, uint8_t& val)
{
	if (node.empty())
	{
		spdlog::error("Node is empty.");
		return false;
	}
	if (!node.isNumeric())
	{
		spdlog::error("Node data type is not numeric.");
		return false;
	}
	val = (uint8_t)node.asInt();
	return true;
}

bool deserialize(const Json::Value& node, uint16_t& val)
{
	if (node.empty())
	{
		spdlog::error("Node is empty.");
		return false;
	}
	if (!node.isNumeric())
	{
		spdlog::error("Node data type is not numeric.");
		return false;
	}
	val = (uint16_t)node.asInt();
	return true;
}

bool deserialize(const Json::Value& node, uint32_t& val)
{
	if (node.empty())
	{
		spdlog::error("Node is empty.");
		return false;
	}
	if (!node.isNumeric())
	{
		spdlog::error("Node data type is not numeric.");
		return false;
	}
	val = (uint32_t)node.asInt();
	return true;
}

bool deserialize(const Json::Value& node, uint64_t& val)
{
	if (node.empty())
	{
		spdlog::error("Node is empty.");
		return false;
	}
	if (!node.isNumeric())
	{
		spdlog::error("Node data type is not numeric.");
		return false;
	}
	val = (uint64_t)node.asInt64();
	return true;
}

bool deserialize(const Json::Value& node, float& f)
{
	if (node.empty())
	{
		spdlog::error("Node is empty.");
		return false;
	}
	if (!node.isNumeric())
	{
		spdlog::error("Node data type is not numeric.");
		return false;
	}
	f = node.asFloat();
	return true;
}

bool deserialize(const Json::Value& node, double& d)
{
	if (node.empty())
	{
		spdlog::error("Node is empty.");
		return false;
	}
	if (!node.isNumeric())
	{
		spdlog::error("Node data type is not numeric.");
		return false;
	}
	d = node.asDouble();
	return true;
}

bool deserialize(const Json::Value& node, std::string& str)
{
	if (node.empty())
	{
		spdlog::error("Node is empty.");
		return false;
	}
    if (!node.isString())
    {
        spdlog::error("Node data type is not string.");
        return false;
    }
    str = node.asString();
    return true;
}

bool deserialize(const Json::Value& node, glm::vec2& vec)
{
	if (node.empty())
	{
		spdlog::error("Node is empty.");
		return false;
	}
	if (!node.isArray())
	{
		spdlog::error("Node data type is not array.");
		return false;
	}
	if (node.size() != 2)
	{
		spdlog::error("Node data type is not array with size 2.");
		return false;
	}

	glm::vec2 temp;
	for (unsigned int i = 0; i < 2; ++i)
	{
		if (!deserialize(node[i], temp[i]))
		{
			spdlog::error("Node data at index {} is invalid.", i);
			return false;
		}
	}

	vec = temp;
	return true;
}

bool deserialize(const Json::Value& node, glm::vec3& vec)
{
	if (node.empty())
	{
		spdlog::error("Node is empty.");
		return false;
	}
	if (!node.isArray())
	{
		spdlog::error("Node data type is not array.");
		return false;
	}
	if (node.size() != 3)
	{
		spdlog::error("Node data type is not array with size 3.");
		return false;
	}

	glm::vec3 temp;
	for (unsigned int i = 0; i < 3; ++i)
	{
		if (!deserialize(node[i], temp[i]))
		{
			spdlog::error("Node data at index {} is invalid.", i);
			return false;
		}
	}

	vec = temp;
	return true;
}

bool deserialize(const Json::Value& node, glm::vec4& vec)
{
	if (node.empty())
	{
		spdlog::error("Node is empty.");
		return false;
	}
	if (!node.isArray())
	{
		spdlog::error("Node data type is not array.");
		return false;
	}
	if (node.size() != 4)
	{
		spdlog::error("Node data type is not array with size 4.");
		return false;
	}

	glm::vec4 temp;
	for (unsigned int i = 0; i < 4; ++i)
	{
		if (!deserialize(node[i], temp[i]))
		{
			spdlog::error("Node data at index {} is invalid.", i);
			return false;
		}
	}

	vec = temp;
	return true;
}

bool deserialize(const Json::Value& node, glm::quat& q)
{
	if (node.empty())
	{
		spdlog::error("Node is empty.");
		return false;
	}
	if (!node.isArray())
	{
		spdlog::error("Node data type is not array.");
		return false;
	}
	if (node.size() != 4)
	{
		spdlog::error("Node data type is not array with size 4.");
		return false;
	}

	glm::quat temp;
	for (unsigned int i = 0; i < 4; ++i)
	{
		if (!deserialize(node[i], temp[i]))
		{
			spdlog::error("Node data at index {} is invalid.", i);
			return false;
		}
	}

	q = temp;
	return true;
}
