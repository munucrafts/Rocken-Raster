#pragma once
#include <string>
#include <unordered_map>
#include <glm/ext/vector_float3.hpp>
#include <AL/al.h>
#include <AL/alc.h>


// Audio Source Class --------------------------------------------------------------------

struct AudioSource
{
private:
    ALuint sourceId = 0;
    ALuint buffer = 0;

private:
    void InitAudioSource();
    void DeleteAudioSource();

public:
    AudioSource() = default;
    ~AudioSource();

    void PlayAudioSource();
    void LoadAudioFile(std::string audioPath, glm::vec3& origin);

    void SetAudioPitch(float pitch);
    void SetAudioVolume(float volume);
    void SetAudioAttenuation(float attenuation);
    void SetAudioOrigin(glm::vec3& origin);
    void SetAudioVelocity(glm::vec3& velocity);
    void PauseAudio();
    void ResumeAudio();
};


// Audio Listener Class ------------------------------------------------------------------


struct AudioListener
{
public:
    AudioListener() = default;
    ~AudioListener() = default;
    void SetListenerLocation(glm::vec3& location);
    void SetListenerVelocity(glm::vec3& velocity);
};


// Audio Master Class --------------------------------------------------------------------


class AudioMaster
{
public:
    static void InitAudioMaster();
    static void ShutdownAudioMaster();

private:
    AudioMaster() = default;
    ~AudioMaster();

private:
    static ALCdevice* alcDevice;
    static ALCcontext* alcContext;
};