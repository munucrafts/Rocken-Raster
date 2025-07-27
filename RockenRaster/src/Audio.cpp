#define DR_WAV_IMPLEMENTATION
#include "Audio.h"
#include "dr_wav.h"
#include <iostream>
#include "Primitives.h"


// Audio Source Class --------------------------------------------------------------------


void AudioSource::PlayAudioSource() const
{
    ALint state;
    alGetSourcei(sourceId, AL_SOURCE_STATE, &state);

    if (state != AL_PLAYING)
    {
        alSourcei(sourceId, AL_BUFFER, buffer);
        alSourcePlay(sourceId);
    }
}

void AudioSource::LoadAudioFile(const std::string& audioPath, const glm::vec3& origin)
{
    unsigned int channels;
    unsigned int sampleRate;
    drwav_uint64 totalPCMFrameCount;

    int16_t* pSampleData = drwav_open_file_and_read_pcm_frames_s16(audioPath.c_str(), &channels, &sampleRate, &totalPCMFrameCount, nullptr);

    if (!pSampleData) return;

    ALenum format;
    if (channels == 1) format = AL_FORMAT_MONO16;
    else if (channels == 2) format = AL_FORMAT_STEREO16;
    else { drwav_free(pSampleData, nullptr); return; }

    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, pSampleData, totalPCMFrameCount * channels * sizeof(int16_t), sampleRate);

    drwav_free(pSampleData, nullptr);

    InitAudioSource();
    PlayAudioSource();
}

void AudioSource::SetAudioPitch(float pitch) const
{
    alSourcef(sourceId, AL_PITCH, pitch);
}

void AudioSource::SetAudioVolume(float volume) const
{
    alSourcef(sourceId, AL_GAIN, volume);
}

void AudioSource::SetAudioAttenuation (float rollOffFactor, float referenceDistance, float maxDistance) const
{
    alSourcef(sourceId, AL_ROLLOFF_FACTOR, rollOffFactor);
    alSourcef(sourceId, AL_REFERENCE_DISTANCE, referenceDistance);
    alSourcef(sourceId, AL_MAX_DISTANCE, maxDistance);
}

void AudioSource::SetAudioOrigin(const glm::vec3& origin) const
{
    alSource3f(sourceId, AL_POSITION, origin.x, origin.y, origin.z);
}

void AudioSource::SetAudioVelocity(const glm::vec3& velocity) const
{
    alSource3f(sourceId, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
}

void AudioSource::PauseAudio() const
{
    alSourcePause(sourceId);
}

void AudioSource::ResumeAudio() const
{
    alSourcePlay(sourceId);
}

void AudioSource::InitAudioSource()
{
    alGenSources(1, &sourceId);
    alSourcei(sourceId, AL_LOOPING, AL_TRUE);
    SetAudioAttenuation(1.0f, 2.0f, 200.0f);
}

void AudioSource::DeleteAudioSource()
{
    alDeleteSources(1, &sourceId);
}

AudioSource::~AudioSource()
{
    DeleteAudioSource();
}


// Audio Listener Class ------------------------------------------------------------------


AudioListener& AudioListener::GetListener()
{
    static AudioListener audioListener;
    return audioListener;
}

void AudioListener::SetListenerLocation(const glm::vec3& location) const
{
    alListener3f(AL_POSITION, location.x, location.y, location.z);
}

void AudioListener::SetListenerVelocity(const glm::vec3& velocity) const
{
    alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
}


// Audio Master Class --------------------------------------------------------------------

AudioMaster& AudioMaster::GetAudioMaster()
{
    static AudioMaster audioMaster;
    return audioMaster;
}

void AudioMaster::InitAudioMaster()
{
    alcDevice = alcOpenDevice(nullptr);
    alcContext = alcCreateContext(alcDevice, nullptr);
    alcMakeContextCurrent(alcContext);
    alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);

    sounds = { "Adventure", "Hopeful", "Guitar" };
}

void AudioMaster::ShutdownAudioMaster()
{
    alcMakeContextCurrent(nullptr);

    if (alcContext) alcDestroyContext(alcContext);
    if (alcDevice) alcCloseDevice(alcDevice);

    alcContext = nullptr;
    alcDevice = nullptr;
}