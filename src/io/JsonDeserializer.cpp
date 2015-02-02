#include "JsonDeserializer.h"

#include <json/json.h>

#include "debug/Log.h"

bool deserialize(const Json::Value& node, bool& b)
{
	if (node.empty())
	{
		LOG_ERROR("Node is empty.");
		return false;
	}
	if (!node.isBool())
	{
		LOG_ERROR("Node data type is not boolean.");
		return false;
	}
	b = node.asBool();
	return true;
}

bool deserialize(const Json::Value& node, int8_t& val)
{
	if (node.empty())
	{
		LOG_ERROR("Node is empty.");
		return false;
	}
	if (!node.isNumeric())
	{
		LOG_ERROR("Node data type is not numeric.");
		return false;
	}
	val = (int8_t)node.asInt();
	return true;
}

bool deserialize(const Json::Value& node, int16_t& val)
{
	if (node.empty())
	{
		LOG_ERROR("Node is empty.");
		return false;
	}
	if (!node.isNumeric())
	{
		LOG_ERROR("Node data type is not numeric.");
		return false;
	}
	val = (int16_t)node.asInt();
	return true;
}

bool deserialize(const Json::Value& node, int32_t& val)
{
	if (node.empty())
	{
		LOG_ERROR("Node is empty.");
		return false;
	}
	if (!node.isNumeric())
	{
		LOG_ERROR("Node data type is not numeric.");
		return false;
	}
	val = (int32_t)node.asInt();
	return true;
}

bool deserialize(const Json::Value& node, int64_t& val)
{
	if (node.empty())
	{
		LOG_ERROR("Node is empty.");
		return false;
	}
	if (!node.isNumeric())
	{
		LOG_ERROR("Node data type is not numeric.");
		return false;
	}
	val = node.asInt64();
	return true;
}

bool deserialize(const Json::Value& node, uint8_t& val)
{
	if (node.empty())
	{
		LOG_ERROR("Node is empty.");
		return false;
	}
	if (!node.isNumeric())
	{
		LOG_ERROR("Node data type is not numeric.");
		return false;
	}
	val = (uint8_t)node.asInt();
	return true;
}

bool deserialize(const Json::Value& node, uint16_t& val)
{
	if (node.empty())
	{
		LOG_ERROR("Node is empty.");
		return false;
	}
	if (!node.isNumeric())
	{
		LOG_ERROR("Node data type is not numeric.");
		return false;
	}
	val = (uint16_t)node.asInt();
	return true;
}

bool deserialize(const Json::Value& node, uint32_t& val)
{
	if (node.empty())
	{
		LOG_ERROR("Node is empty.");
		return false;
	}
	if (!node.isNumeric())
	{
		LOG_ERROR("Node data type is not numeric.");
		return false;
	}
	val = (uint32_t)node.asInt();
	return true;
}

bool deserialize(const Json::Value& node, uint64_t& val)
{
	if (node.empty())
	{
		LOG_ERROR("Node is empty.");
		return false;
	}
	if (!node.isNumeric())
	{
		LOG_ERROR("Node data type is not numeric.");
		return false;
	}
	val = (uint64_t)node.asInt64();
	return true;
}

bool deserialize(const Json::Value& node, float& f)
{
	if (node.empty())
	{
		LOG_ERROR("Node is empty.");
		return false;
	}
	if (!node.isNumeric())
	{
		LOG_ERROR("Node data type is not numeric.");
		return false;
	}
	f = node.asFloat();
	return true;
}

bool deserialize(const Json::Value& node, double& d)
{
	if (node.empty())
	{
		LOG_ERROR("Node is empty.");
		return false;
	}
	if (!node.isNumeric())
	{
		LOG_ERROR("Node data type is not numeric.");
		return false;
	}
	d = node.asDouble();
	return true;
}

bool deserialize(const Json::Value& node, std::string& str)
{
	if (node.empty())
	{
		LOG_ERROR("Node is empty.");
		return false;
	}
    if (!node.isString())
    {
        LOG_ERROR("Node data type is not string.");
        return false;
    }
    str = node.asString();
    return true;
}

bool deserialize(const Json::Value& node, glm::vec2& vec)
{
	if (node.empty())
	{
		LOG_ERROR("Node is empty.");
		return false;
	}
	if (!node.isArray())
	{
		LOG_ERROR("Node data type is not array.");
		return false;
	}
	if (node.size() != 2)
	{
		LOG_ERROR("Node data type is not array with size 2.");
		return false;
	}

	glm::vec2 temp;
	for (unsigned int i = 0; i < 2; ++i)
	{
		if (!deserialize(node[i], temp[i]))
		{
			LOG_ERROR("Node data at index %u is invalid.", i);
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
		LOG_ERROR("Node is empty.");
		return false;
	}
	if (!node.isArray())
	{
		LOG_ERROR("Node data type is not array.");
		return false;
	}
	if (node.size() != 3)
	{
		LOG_ERROR("Node data type is not array with size 3.");
		return false;
	}

	glm::vec3 temp;
	for (unsigned int i = 0; i < 3; ++i)
	{
		if (!deserialize(node[i], temp[i]))
		{
			LOG_ERROR("Node data at index %u is invalid.", i);
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
		LOG_ERROR("Node is empty.");
		return false;
	}
	if (!node.isArray())
	{
		LOG_ERROR("Node data type is not array.");
		return false;
	}
	if (node.size() != 4)
	{
		LOG_ERROR("Node data type is not array with size 4.");
		return false;
	}

	glm::vec4 temp;
	for (unsigned int i = 0; i < 4; ++i)
	{
		if (!deserialize(node[i], temp[i]))
		{
			LOG_ERROR("Node data at index %u is invalid.", i);
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
		LOG_ERROR("Node is empty.");
		return false;
	}
	if (!node.isArray())
	{
		LOG_ERROR("Node data type is not array.");
		return false;
	}
	if (node.size() != 4)
	{
		LOG_ERROR("Node data type is not array with size 4.");
		return false;
	}

	glm::quat temp;
	for (unsigned int i = 0; i < 4; ++i)
	{
		if (!deserialize(node[i], temp[i]))
		{
			LOG_ERROR("Node data at index %u is invalid.", i);
			return false;
		}
	}

	q = temp;
	return true;
}