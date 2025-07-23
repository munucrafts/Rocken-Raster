#define DR_WAV_IMPLEMENTATION
#include "Audio.h"
#include "dr_wav.h"
#include <iostream>

void AudioSource::Play()
{
    alSourcei(sourceId, AL_BUFFER, buffer);
    alSourcePlay(sourceId);
    std::cout << "Playing - " << sourceId << std::endl;
}

void AudioSource::InitSource()
{
    alGenSources(1, &sourceId);
    alSourcef(sourceId, AL_PITCH, pitch);
    alSourcef(sourceId, AL_GAIN, volume);
    alSource3f(sourceId, AL_POSITION, origin.x, origin.y, origin.z);
}

AudioSource::AudioSource()
{

}

AudioSource::~AudioSource()
{
    alDeleteSources(1, &sourceId);
}

AudioListener::AudioListener()
{
    alListener3f(AL_POSITION, 0, 0, 0);
    alListener3f(AL_VELOCITY, 0, 0, 0);
}

void AudioComponent::LoadAudioFile(std::string audioPath)
{
    unsigned int channels;
    unsigned int sampleRate;
    drwav_uint64 totalPCMFrameCount;

    int16_t* pSampleData = drwav_open_file_and_read_pcm_frames_s16(audioPath.c_str(), &channels, &sampleRate, &totalPCMFrameCount, nullptr);

    if (!pSampleData) return;
  
    ALenum format;
    if (channels == 1) format = AL_FORMAT_MONO16;
    else if (channels == 2) format = AL_FORMAT_STEREO16;
    else {drwav_free(pSampleData, nullptr); return;}

    alGenBuffers(1, &(audioSource.buffer));
    alBufferData(audioSource.buffer, format, pSampleData, totalPCMFrameCount * channels * sizeof(int16_t), sampleRate);

    drwav_free(pSampleData, nullptr);
}

void AudioComponent::PlayAudio()
{
    audioSource.Play();
}
