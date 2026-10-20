#include "audio/SoundEngine.h"

#include <spdlog/spdlog.h>

#include "resource/IResourceManager.h"
#include "resource/SoundData.h"

namespace
{
const SoundData* getPlayableSound(IResourceManager& resourceManager, ResourceId id)
{
    const SoundData* data = resourceManager.getSound(id);
    if (data == nullptr || data->m_samples.empty() || data->m_channels == 0)
    {
        return nullptr;
    }
    return data;
}

bool createSound(ma_engine& engine, ResourceId id, const SoundData& data, ma_audio_buffer& buffer,
                 ma_sound& sound, ma_uint32 flags)
{
    ma_audio_buffer_config bufferConfig = ma_audio_buffer_config_init(
        ma_format_s16, data.m_channels, data.m_samples.size() / data.m_channels,
        data.m_samples.data(), nullptr);
    bufferConfig.sampleRate = data.m_sampleRate;
    if (ma_audio_buffer_init(&bufferConfig, &buffer) != MA_SUCCESS)
    {
        spdlog::error("Failed to create the audio buffer for sound {}.", id);
        return false;
    }

    if (ma_sound_init_from_data_source(&engine, &buffer, flags, nullptr, &sound) != MA_SUCCESS)
    {
        spdlog::error("Failed to create the sound object for sound {}.", id);
        ma_audio_buffer_uninit(&buffer);
        return false;
    }

    ma_sound_set_looping(&sound, MA_TRUE);
    return true;
}

void destroySound(ma_audio_buffer& buffer, ma_sound& sound)
{
    ma_sound_stop(&sound);
    ma_sound_uninit(&sound);
    ma_audio_buffer_uninit(&buffer);
}
}  // namespace

SoundEngine::SoundEngine() { return; }

SoundEngine::~SoundEngine()
{
    stopAll();
    if (m_engineReady)
    {
        ma_engine_uninit(&m_engine);
    }
}

bool SoundEngine::init()
{
    if (ma_engine_init(nullptr, &m_engine) != MA_SUCCESS)
    {
        spdlog::error("Failed to initialize the audio engine.");
        return false;
    }
    m_engineReady = true;
    return true;
}

void SoundEngine::setVolume(float volume)
{
    if (m_engineReady)
    {
        ma_engine_set_volume(&m_engine, volume);
    }
}

void SoundEngine::stopAll()
{
    if (m_music.has_value())
    {
        destroySound(m_music->m_buffer, m_music->m_sound);
        m_music.reset();
    }
    for (PlayingSound& emitter : m_emitters)
    {
        destroySound(emitter.m_buffer, emitter.m_sound);
    }
    m_emitters.clear();
}

void SoundEngine::playMusic(ResourceId id, IResourceManager& resourceManager)
{
    if (!m_engineReady)
    {
        return;
    }

    const SoundData* data = getPlayableSound(resourceManager, id);
    if (data == nullptr)
    {
        return;
    }

    if (m_music.has_value())
    {
        destroySound(m_music->m_buffer, m_music->m_sound);
        m_music.reset();
    }

    PlayingSound& music = m_music.emplace();
    if (!createSound(m_engine, id, *data, music.m_buffer, music.m_sound,
                     MA_SOUND_FLAG_NO_SPATIALIZATION))
    {
        m_music.reset();
        return;
    }

    if (ma_sound_start(&music.m_sound) != MA_SUCCESS)
    {
        spdlog::error("Failed to start background music {}.", id);
        destroySound(music.m_buffer, music.m_sound);
        m_music.reset();
        return;
    }
    spdlog::info("Playing background music {} in a loop.", id);
}

bool SoundEngine::addEmitter(ResourceId id, const glm::vec3& position, float radius,
                             IResourceManager& resourceManager)
{
    if (!m_engineReady)
    {
        return false;
    }

    const SoundData* data = getPlayableSound(resourceManager, id);
    if (data == nullptr)
    {
        return false;
    }

    PlayingSound& emitter = m_emitters.emplace_back();
    if (!createSound(m_engine, id, *data, emitter.m_buffer, emitter.m_sound, 0))
    {
        m_emitters.pop_back();
        return false;
    }

    ma_sound_set_attenuation_model(&emitter.m_sound, ma_attenuation_model_linear);
    // The gain falls off from the source to the radius, without a minimum
    // distance it would stay at full volume around the emitter.
    ma_sound_set_min_distance(&emitter.m_sound, 0.0f);
    ma_sound_set_max_distance(&emitter.m_sound, radius);
    ma_sound_set_position(&emitter.m_sound, position.x, position.y, position.z);

    if (ma_sound_start(&emitter.m_sound) != MA_SUCCESS)
    {
        spdlog::error("Failed to start the sound emitter for sound {}.", id);
        destroySound(emitter.m_buffer, emitter.m_sound);
        m_emitters.pop_back();
        return false;
    }
    spdlog::info("Started sound emitter for sound {} at ({}, {}, {}).", id, position.x, position.y,
                 position.z);
    return true;
}

void SoundEngine::updateListener(const glm::vec3& position, const glm::vec3& forward)
{
    if (!m_engineReady)
    {
        return;
    }
    ma_engine_listener_set_position(&m_engine, 0, position.x, position.y, position.z);
    ma_engine_listener_set_direction(&m_engine, 0, forward.x, forward.y, forward.z);
}
