#pragma once
#include <string>
#include <unordered_map>

struct AudioLibrary
{
public:
	std::unordered_map<std::string, std::string> audios;

public:
	static AudioLibrary& GetAudioLib();

private:
	AudioLibrary();
	~AudioLibrary() = default;
	AudioLibrary(const AudioLibrary&) = delete;
	AudioLibrary& operator= (const AudioLibrary&) = delete;
};

struct AudioComponent
{
public:
	AudioComponent();
	void PlayAudio();
	void LoadAudioFile(std::string audioName);

public:
	float attenuation;
	float pitch;

private:
	int audioBuffer;
};