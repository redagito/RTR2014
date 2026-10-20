#pragma once

#include <cstdint>

typedef int64_t ResourceId;
constexpr ResourceId invalidResource = -1;

/**
 * \brief Resource types.
 */
enum class ResourceType
{
    Mesh,
    Image,
    String,
    Shader,
    Material,
    Sound
};

/**
* \brief Primitive type for vertex data.
* NOTE GL_QUAD is deprecated.
*/
enum class PrimitiveType
{
    Point = 1,
    Line = 2,
    Triangle = 3,
    Invalid
};

/**
* \brief Color format for texture data.
*/
enum class ColorFormat
{
    GreyScale8 = 1,
    RGB24 = 3,
    RGBA32 = 4,
    Invalid
};

/**
 * \brief Possible listener events.
 */
enum class ListenerEvent
{
    Create, /**< Specified resource was created. */
    Change, /**< Specified resource was changed. */
    Delete  /**< Specified resource was deleted. */
};
