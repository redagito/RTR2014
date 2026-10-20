#pragma once

#include <cstdint>
#include <vector>

/**
 * \brief Decoded sound data as interleaved 16-bit pcm samples.
 */
struct SoundData
{
    std::vector<int16_t> m_samples;
    unsigned int m_sampleRate = 0;
    unsigned int m_channels = 0;
};
