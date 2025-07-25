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

public:
    AudioSource() = default;
    ~AudioSource();
    void InitAudioSource();
    void DeleteAudioSource();
    void PlayAudioSource();
    void LoadAudioFile(std::string audioPath, glm::vec3& origin);
    void SetAudioPitch(float pitch);
    void SetAudioVolume(float volume);
    void SetAudioAttenuation(float rollOffFactor, float referenceDistance, float maxDistance);
    void SetAudioOrigin(glm::vec3& origin);
    void SetAudioVelocity(glm::vec3& velocity);
    void PauseAudio();
    void ResumeAudio();
};


// Audio Listener Class ------------------------------------------------------------------


struct AudioListener
{
public:
    static AudioListener& GetListener();
    void SetListenerLocation(glm::vec3& location);
    void SetListenerVelocity(glm::vec3& velocity);

private:
    AudioListener() = default;
    ~AudioListener() = default;
};


// Audio Master Class --------------------------------------------------------------------


class AudioMaster
{
public:
    static AudioMaster& GetAudioMaster();
    void InitAudioMaster();
    void ShutdownAudioMaster();

private:
    ALCdevice* alcDevice;
    ALCcontext* alcContext;

private:
    AudioMaster() = default;
    ~AudioMaster() = default;
};