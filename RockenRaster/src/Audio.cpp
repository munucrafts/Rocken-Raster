#define DR_WAV_IMPLEMENTATION
#include "Audio.h"
#include "dr_wav.h"
#include <iostream>
#include "Primitives.h"


// Audio Source Class --------------------------------------------------------------------


void AudioSource::PlayAudioSource()
{
    ALint state;
    alGetSourcei(sourceId, AL_SOURCE_STATE, &state);

    if (state != AL_PLAYING)
    {
        alSourcei(sourceId, AL_BUFFER, buffer);
        alSourcePlay(sourceId);
    }
}

void AudioSource::LoadAudioFile(std::string audioPath, glm::vec3& origin)
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

    audioOrigin = origin;
    InitAudioSource();
}

void AudioSource::SetAudioPitch(float pitch)
{
    audioPitch = pitch;
    alSourcef(sourceId, AL_PITCH, audioPitch);
}

void AudioSource::SetAudioVolume(float volume)
{
    audioVolume = volume;
    alSourcef(sourceId, AL_GAIN, audioVolume);
}

void AudioSource::SetAudioAttenuation(float attenuation)
{
    audioAttenuation = attenuation;
    //alSourcef(sourceId, AL_)
}

void AudioSource::SetAudioOrigin(glm::vec3& origin)
{
    audioOrigin = origin;
    alSource3f(sourceId, AL_POSITION, audioOrigin.x, audioOrigin.y, audioOrigin.z);
}

void AudioSource::SetAudioVelocity(glm::vec3& velocity)
{
    audioVelocity = velocity;
    alSource3f(sourceId, AL_VELOCITY, audioVelocity.x, audioVelocity.y, audioVelocity.z);
}

void AudioSource::InitAudioSource()
{
    alGenSources(1, &sourceId);
    alSourcef(sourceId, AL_GAIN, audioVolume);
    alSourcef(sourceId, AL_PITCH, audioPitch);
    alSource3f(sourceId, AL_POSITION, audioOrigin.x, audioOrigin.y, audioOrigin.z);
    alSource3f(sourceId, AL_VELOCITY, audioVelocity.x, audioVelocity.y, audioVelocity.z);
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


AudioListener::AudioListener()
{
    alListener3f(AL_POSITION, 10, 0, 0);
    alListener3f(AL_VELOCITY, 0, 0, 0);
}


// Audio Master Class --------------------------------------------------------------------


ALCdevice* AudioMaster::alcDevice = nullptr;
ALCcontext* AudioMaster::alcContext = nullptr;

void AudioMaster::InitAudioMaster()
{
    alcDevice = alcOpenDevice(nullptr);
    alcContext = alcCreateContext(alcDevice, nullptr);
    alcMakeContextCurrent(alcContext);
}

void AudioMaster::ShutdownAudioMaster()
{
    alcMakeContextCurrent(nullptr);
    if (alcContext) alcDestroyContext(alcContext);
    if (alcDevice) alcCloseDevice(alcDevice);

    alcContext = nullptr;
    alcDevice = nullptr;
}

AudioMaster::~AudioMaster()
{
    ShutdownAudioMaster();
}
