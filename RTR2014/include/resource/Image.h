#pragma once

#include <vector>

#include "resource/ResourceConfig.h"

/**
 * \brief Image data.
 */
struct Image
{
    Image();
    Image(std::vector<unsigned char> data, unsigned int width, unsigned int height,
          ColorFormat format);
    std::vector<unsigned char> m_data;
    unsigned int m_width;
    unsigned int m_height;
    ColorFormat m_format;
};
