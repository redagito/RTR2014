#pragma once

#include <algorithm>

/**
 * \brief Measures the time between frames.
 *
 * The delta is clamped, so that a single long frame does not move the
 * simulation further than the clamp allows.
 */
class FrameTimer
{
   public:
    explicit FrameTimer(double startTime) : m_previousTime(startTime) {}

    /**
     * \brief Returns the time elapsed since the previous call and advances the timer.
     */
    float tick(double now)
    {
        const float delta = static_cast<float>(now - m_previousTime);
        m_previousTime = now;
        return std::min(delta, kMaxDelta);
    }

   private:
    static constexpr float kMaxDelta = 0.1f;

    double m_previousTime;
};
