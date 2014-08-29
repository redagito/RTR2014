#pragma once

#include <cstdint>

typedef int64_t ResourceId;
static const ResourceId invalidResource = -1;

/**
 * \brief Resource types.
 */
enum class EResourceType
{
    Mesh,
    Image,
    String,
    Shader,
    Material
};

/**
* \brief Primitive type for vertex data.
*/
enum class EPrimitiveType
{
    Point,
    Line,
    Triangle,
    Quad,
    Invalid
};

/**
* \brief Color format for texture data.
*/
enum class EColorFormat
{
    GreyScale8,
    RGB24,
    RGBA32,
    Invalid
};

/**
 * \brief Possible listener events.
 */
enum class EListenerEvent
{
    Create, /**< Specified resource was created. */
    Change, /**< Specified resource was changed. */
    Delete  /**< Specified resource was deleted. */
};
