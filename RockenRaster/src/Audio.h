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

    float audioAttenuation = 10.0f;
    float audioPitch = 1.0f;
    float audioVolume = 1.0f;

    glm::vec3 audioOrigin = glm::vec3(0.0f);
    glm::vec3 audioVelocity = glm::vec3(0.0f);

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
};


// Audio Listener Class ------------------------------------------------------------------


struct AudioListener
{
    AudioListener();
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