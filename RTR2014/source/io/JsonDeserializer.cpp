#include <limits>

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

template <typename T>
    requires std::integral<T> && (!std::same_as<T, bool>)
bool deserialize(const Json::Value& node, T& val)
{
	if (node.empty())
	{
		spdlog::error("Node is empty.");
		return false;
	}
	if (!node.isIntegral())
	{
		spdlog::error("Node data type is not integral.");
		return false;
	}

	if constexpr (std::is_signed_v<T>)
	{
		// jsoncpp converts to a signed 64 bit value, which fails for the
		// unsigned values that do not fit into it.
		if (node.isUInt64() &&
			node.asUInt64() > static_cast<uint64_t>(std::numeric_limits<int64_t>::max()))
		{
			spdlog::error("Node value does not fit into a signed integer.");
			return false;
		}

		const int64_t raw = node.asInt64();
		if (raw < static_cast<int64_t>(std::numeric_limits<T>::min()) ||
			raw > static_cast<int64_t>(std::numeric_limits<T>::max()))
		{
			spdlog::error("Node value {} does not fit in the target type.", raw);
			return false;
		}

		val = static_cast<T>(raw);
	}
	else
	{
		if (node.isInt64() && node.asInt64() < 0)
		{
			spdlog::error("Node value {} is negative.", node.asInt64());
			return false;
		}

		const uint64_t raw =
			node.isUInt64() ? node.asUInt64() : static_cast<uint64_t>(node.asInt64());
		if (raw > static_cast<uint64_t>(std::numeric_limits<T>::max()))
		{
			spdlog::error("Node value {} does not fit in the target type.", raw);
			return false;
		}

		val = static_cast<T>(raw);
	}
	return true;
}

template bool deserialize(const Json::Value&, int8_t&);
template bool deserialize(const Json::Value&, int16_t&);
template bool deserialize(const Json::Value&, int32_t&);
template bool deserialize(const Json::Value&, int64_t&);
template bool deserialize(const Json::Value&, uint8_t&);
template bool deserialize(const Json::Value&, uint16_t&);
template bool deserialize(const Json::Value&, uint32_t&);
template bool deserialize(const Json::Value&, uint64_t&);

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

bool loadParam(const Json::Value& node, const std::string& name, std::string& str)
{
    if (!node.isObject())
    {
        spdlog::error("Node is not an object while loading '{}' parameter.", name.c_str());
        return false;
    }
    if (!deserialize(node[name], str))
    {
        spdlog::error("Failed to load '{}' parameter.", name.c_str());
        return false;
    }
    return true;
}

bool loadParam(const Json::Value& node, const std::string& name, glm::vec3& vec)
{
    if (!node.isObject())
    {
        spdlog::error("Node is not an object while loading '{}' parameter.", name.c_str());
        return false;
    }
    if (!deserialize(node[name], vec))
    {
        spdlog::error("Failed to load '{}' parameter.", name.c_str());
        return false;
    }
    return true;
}

bool loadParam(const Json::Value& node, const std::string& name, float& f)
{
    if (!node.isObject())
    {
        spdlog::error("Node is not an object while loading '{}' parameter.", name.c_str());
        return false;
    }
    if (!deserialize(node[name], f))
    {
        spdlog::error("Failed to load '{}' parameter.", name.c_str());
        return false;
    }
    return true;
}

bool loadParam(const Json::Value& node, const std::string& name, bool& b)
{
    if (!node.isObject())
    {
        spdlog::error("Node is not an object while loading '{}' parameter.", name.c_str());
        return false;
    }
    if (!deserialize(node[name], b))
    {
        spdlog::error("Failed to load '{}' parameter.", name.c_str());
        return false;
    }
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
