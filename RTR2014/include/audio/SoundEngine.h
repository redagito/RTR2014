#pragma once

#include <list>
#include <optional>

#include <glm/glm.hpp>
#include <miniaudio.h>

#include "resource/ResourceConfig.h"

class IResourceManager;

/**
* \brief Plays decoded sound resources through miniaudio.
* Background music plays in 2D; emitters are positional and attenuated by distance.
*/
class SoundEngine
{
   public:
    SoundEngine();
    ~SoundEngine();

    SoundEngine(const SoundEngine&) = delete;
    SoundEngine& operator=(const SoundEngine&) = delete;

    /**
    * \brief Initializes the audio engine and playback device.
    * Returns false and leaves the engine silent when no device is available.
    */
    bool init();

    /**
    * \brief Sets the master volume, applied to current and future sounds.
    */
    void setVolume(float volume);

    /**
    * \brief Stops and releases all playing sounds.
    */
    void stopAll();

    /**
    * \brief Plays the sound as non-positional background music in a loop.
    */
    void playMusic(ResourceId id, IResourceManager& resourceManager);

    /**
    * \brief Starts a looping positional sound at the given position.
    * Its gain falls linearly to zero at radius distance from the listener.
    * Returns false when the engine is silent or the sound cannot be played.
    */
    bool addEmitter(ResourceId id, const glm::vec3& position, float radius,
                    IResourceManager& resourceManager);

    /**
    * \brief Moves the audio listener to the camera.
    */
    void updateListener(const glm::vec3& position, const glm::vec3& forward);

   private:
    struct PlayingSound
    {
        ma_audio_buffer m_buffer;
        ma_sound m_sound;
    };

    ma_engine m_engine;
    bool m_engineReady = false;
    std::optional<PlayingSound> m_music;
    std::list<PlayingSound> m_emitters; /**< Node-based: miniaudio forbids moving ma_sound. */
};
