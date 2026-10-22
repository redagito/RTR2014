#pragma once

#include <cstdint>
#include <string>

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
 * \brief Color space of the values in a texture or a color.
 *
 * Linear values are used for everything that is not a color, a direction or a
 * scalar. sRGB values are the encoded form of a color, they have to be decoded
 * before they are used in a calculation.
 */
enum class ColorSpace
{
    Linear,
    sRGB
};

/**
 * \brief An image file with the format and color space it is used with.
 */
struct ImageDescriptor
{
    std::string m_file;
    ColorFormat m_format;
    ColorSpace m_colorSpace;
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
