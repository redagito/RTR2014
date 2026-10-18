#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <cassert>

/**
 * \brief Generic sampler class.
 * Stores samples with keys, which impose a total ordering on the samples.
 */
template <typename SampleType, typename KeyType>
class TSampler
{
   public:
    typedef std::function<SampleType(KeyType key0, SampleType value0, KeyType key1,
                                     SampleType value1, KeyType key2, SampleType value2,
                                     KeyType key)> InterpolationFunction;

    /**
     * \brief Allocates initial sample vector storage.
     * Set the size according to expected number of samples.
     */
    TSampler(InterpolationFunction interpolFunc, unsigned int initSizeAlloc = 1024);

    /**
     * \brief Adds sample.
     */
    void addSample(SampleType value, KeyType key);

    /**
     * \brief Returns interpolated sample for the specified key.
     */
    SampleType getSample(KeyType key) const;

   private:
    /**
     * \brief Sample data.
     */
    struct SSample
    {
        SampleType value; /**< Sampled value. */
        KeyType key;      /**< Sample key. */

        SSample(SampleType v, KeyType k) : value(v), key(k) {}
        bool operator<(const SSample& rhs) const { return key < rhs.key; }
    };

    std::vector<SSample> m_samples; /**< Current samples. */
    const InterpolationFunction m_interpolationFunction;

    // Cached indices of
    mutable unsigned int m_currIndex; /**< Previous sample. */
};

template <typename SampleType, typename KeyType>
TSampler<SampleType, KeyType>::TSampler(InterpolationFunction interpolFunc,
                                        unsigned int initSizeAlloc)
    : m_interpolationFunction(interpolFunc), m_currIndex(0)
{
    m_samples.reserve(initSizeAlloc);
}

template <typename SampleType, typename KeyType>
void TSampler<SampleType, KeyType>::addSample(SampleType value, KeyType key)
{
    bool sorted = false;
    TSampler<SampleType, KeyType>::SSample sample(key, value);

    if (!m_samples.empty())
    {
        // Needs sort after add?
        if (m_samples.back() < sample)
        {
            sorted = true;
        }
    }
    m_samples.push_back(sample);

    // Sort if needed
    if (!sorted)
    {
        std::sort(m_samples.begin(), m_samples.end());
        // Reset index after sort
        m_currIndex = 0;
    }
}

template <typename SampleType, typename KeyType>
SampleType TSampler<SampleType, KeyType>::getSample(KeyType key) const
{
    assert(!m_samples.empty());
    assert(m_samples.size() >= 3);

    while (m_currIndex < m_samples.size())
    {
        // Search for first sample with greater key
        if (m_samples.at(m_currIndex).key > key)
        {
            // First sample already bigger
            if (m_currIndex == 0)
            {
                // Return first sample
                return m_samples.at(0).value;
            }

            if (m_samples.size() > m_currIndex + 1)
            {
                // 3 samples
                return m_interpolationFunction(
                    m_samples.at(m_currIndex - 1).key, m_samples.at(m_currIndex - 1).value,
                    m_samples.at(m_currIndex).key, m_samples.at(m_currIndex).value,
                    m_samples.at(m_currIndex + 1).key, m_samples.at(m_currIndex + 1).value, key);
            }
            else
            {
                return m_interpolationFunction(
                    m_samples.at(m_currIndex - 2).key, m_samples.at(m_currIndex - 2).value,
                    m_samples.at(m_currIndex - 1).key, m_samples.at(m_currIndex - 1).value,
                    m_samples.at(m_currIndex).key, m_samples.at(m_currIndex).value, key);
            }
        }
        // Step next
        ++m_currIndex;
    }

    // Key is greater than last sample, step back
    --m_currIndex;
    // Return last sample value
    return m_samples.at(m_currIndex).value;
}
