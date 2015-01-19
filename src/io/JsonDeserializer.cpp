#include "JsonDeserializer.h"

#include "debug/Log.h"

bool deserialize(const Json::Value& node, bool& b)
{
    // TODO comment code back in, after monday.
//	if (node.empty())
//	{
//		LOG_ERROR("Node is empty.");
//		return false;
//	}
//	if (!node.isBool())
//	{
//		LOG_ERROR("Node data type is not boolean.");
//		return false;
//	}
	b = node.asBool();
	return true;
}

bool deserialize(const Json::Value& node, int64_t& l)
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
    l = node.asInt64();
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