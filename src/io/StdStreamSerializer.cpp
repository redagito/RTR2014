#include "StdStreamSerializer.h"

#include <glm/ext.hpp>

template <typename T>
bool serializeNative(std::ostream& stream, const T* value, uint64_t size)
{
    stream.write(reinterpret_cast<const char*>(value), sizeof(T) * size);
    return !stream.fail();
}

template <typename T>
bool serializeNative(std::ostream& stream, T value)
{
	return serializeNative(stream, &value, 1);
}

template <typename T>
bool serializeNativeVector(std::ostream& stream, const std::vector<T>& value)
{
	uint64_t size = value.size();
	return serialize(stream, size) && serializeNative(stream, value.data(), size);
}

template <typename T>
bool serializeVector(std::ostream& stream, const std::vector<T>& value)
{
	// Write size
	uint64_t size = value.size();
	if (!serialize(stream, size))
	{
		return false;
	}

	// Write data
	for (const auto& val : value)
	{
		if (!serialize(stream, val))
		{
			return false;
		}
	}
	return true;
}

bool serialize(std::ostream& stream, bool value) { return serializeNative(stream, value); }

bool serialize(std::ostream& stream, int8_t value) { return serializeNative(stream, value); }

bool serialize(std::ostream& stream, int16_t value) { return serializeNative(stream, value); }

bool serialize(std::ostream& stream, int32_t value) { return serializeNative(stream, value); }

bool serialize(std::ostream& stream, int64_t value) { return serializeNative(stream, value); }

bool serialize(std::ostream& stream, uint8_t value) { return serializeNative(stream, value); }

bool serialize(std::ostream& stream, uint16_t value) { return serializeNative(stream, value); }

bool serialize(std::ostream& stream, uint32_t value) { return serializeNative(stream, value); }

bool serialize(std::ostream& stream, uint64_t value) { return serializeNative(stream, value); }

bool serialize(std::ostream& stream, float value) { return serializeNative(stream, value); }

bool serialize(std::ostream& stream, double value) { return serializeNative(stream, value); }

bool serialize(std::ostream& stream, const glm::vec2& value)
{
	return serializeNative(stream, glm::value_ptr(value), 2);
}

bool serialize(std::ostream& stream, const glm::vec3& value)
{
	return serializeNative(stream, glm::value_ptr(value), 3);
}

bool serialize(std::ostream& stream, const glm::vec4& value)
{
	return serializeNative(stream, glm::value_ptr(value), 4);
}

bool serialize(std::ostream& stream, const glm::mat2& value)
{
	return serializeNative(stream, glm::value_ptr(value), 4);
}

bool serialize(std::ostream& stream, const glm::mat3& value)
{
	return serializeNative(stream, glm::value_ptr(value), 9);
}

bool serialize(std::ostream& stream, const glm::mat4& value)
{
	return serializeNative(stream, glm::value_ptr(value), 16);
}

bool serialize(std::ostream& stream, const std::string& value)
{
    uint64_t size = value.size();
	return serializeNative(stream, size) && serializeNative(stream, value.data(), size);
}

bool serialize(std::ostream& stream, const std::vector<uint8_t>& value)
{
	return serializeNativeVector(stream, value);
}

bool serialize(std::ostream& stream, const std::vector<uint16_t>& value)
{
	return serializeNativeVector(stream, value);
}

bool serialize(std::ostream& stream, const std::vector<uint32_t>& value)
{
	return serializeNativeVector(stream, value);
}

bool serialize(std::ostream& stream, const std::vector<uint64_t>& value)
{
	return serializeNativeVector(stream, value);
}

bool serialize(std::ostream& stream, const std::vector<float>& value)
{
	return serializeNativeVector(stream, value);
}

bool serialize(std::ostream& stream, const std::vector<glm::vec3>& value)
{
	return serializeVector(stream, value);
}