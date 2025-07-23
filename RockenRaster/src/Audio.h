#pragma once
#include <string>
#include <unordered_map>
#include <glm/ext/vector_float3.hpp>
#include <AL/al.h>
#include <AL/alc.h>

struct AudioSource
{
    ALuint sourceId = 0;
    ALuint buffer = 0;
    float attenuation = 10.0f;
    float pitch = 1.0f;
    float volume = 1.0f;
    glm::vec3 origin = glm::vec3(0.0f);
    glm::vec3 velocity = glm::vec3(0.0f);

    AudioSource();
    ~AudioSource();
    void Play();
    void InitSource();
};

struct AudioListener
{
    AudioListener();
};

struct AudioComponent
{
public:
	AudioComponent() = default;
	~AudioComponent() = default;
	void LoadAudioFile(std::string audioPath);
    void PlayAudio();

private:
	AudioSource audioSource;
    AudioListener audioListener;
};