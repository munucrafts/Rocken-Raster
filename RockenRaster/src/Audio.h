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
    void PlayAudioSource() const;
    void LoadAudioFile(const std::string& audioPath, const glm::vec3& origin);
    void SetAudioPitch(const float pitch) const;
    void SetAudioVolume(const float volume) const;
    void SetAudioAttenuation(const float rollOffFactor, const float referenceDistance, const float maxDistance) const;
    void SetAudioOrigin(const glm::vec3& origin) const;
    void SetAudioVelocity(const glm::vec3& velocity) const;
    void PauseAudio() const;
    void ResumeAudio() const;
};


// Audio Listener Class ------------------------------------------------------------------


struct AudioListener
{
public:
    static AudioListener& GetListener();
    void SetListenerLocation(const glm::vec3& location) const;
    void SetListenerVelocity(const glm::vec3& velocity) const;
    void SetListenerOrientation(const glm::vec3& forward, const glm::vec3& up) const;

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

public:
    std::vector<std::string> sounds;

private:
    ALCdevice* alcDevice;
    ALCcontext* alcContext;

private:
    AudioMaster();
    ~AudioMaster() = default;
};