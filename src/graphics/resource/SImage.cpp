#include "SImage.h"

SImage::SImage(std::vector<unsigned char> data, unsigned int width, unsigned int height,
               EColorFormat format)
    : m_data(data), m_width(width), m_height(height), m_format(format)
{
    return;
}

SImage::SImage() : m_width(0), m_height(0), m_format(EColorFormat::Invalid) { return; }
