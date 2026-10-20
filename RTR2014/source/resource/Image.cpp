#include "resource/Image.h"

Image::Image(std::vector<unsigned char> data, unsigned int width, unsigned int height,
             ColorFormat format)
    : m_data(std::move(data)), m_width(width), m_height(height), m_format(format)
{
    return;
}

Image::Image() : m_width(0), m_height(0), m_format(ColorFormat::Invalid) { return; }
