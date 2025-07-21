#include "Audio.h"
#include <iostream>

AudioComponent::AudioComponent()
{
	attenuation = 10.0f;
	pitch = 1.0f;
	origin = glm::vec3(0.0f);
	audioBuffer = NULL;
}

void AudioComponent::PlayAudio()
{
	// Play Audio from Buffer
	std::cout << "Playing " << audioBuffer << std::endl;
}

void AudioComponent::LoadAudioFile(std::string audioName)
{
	AudioLibrary& audioLib = AudioLibrary::GetAudioLib();
	auto iter = audioLib.audios.find(audioName);

	if (iter != audioLib.audios.end())
	{
		std::string& path = iter->second;
		
		// Load Audio from File to Buffer
		audioBuffer = 10;
	}
}

AudioLibrary& AudioLibrary::GetAudioLib()
{
	static AudioLibrary instance;
	return instance;
}

AudioLibrary::AudioLibrary()
{
	audios = {{"fan", "Assets/Fan.wav"}};
}


