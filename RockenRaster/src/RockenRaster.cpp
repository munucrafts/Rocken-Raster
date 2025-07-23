#include <Walnut/Application.h>
#include <Walnut/EntryPoint.h>
#include <Walnut/Timer.h>
#include <Walnut/Image.h>
#include <AL/al.h>
#include <AL/alc.h>
#include "Renderer.h"

class RockenRaster : public Walnut::Layer
{
private:
	Renderer renderer;
	float MS = 0.0f;
	float FPS = 0.0f;

	ALCdevice* alcDevice;
	ALCcontext* alcContext;

private:
	void InitOpenAlAudio()
	{
		alcDevice = alcOpenDevice(nullptr);
		alcContext = alcCreateContext(alcDevice, nullptr);
		alcMakeContextCurrent(alcContext);
	}

public:
	RockenRaster()
	{
		InitOpenAlAudio();
	}
	virtual void OnUIRender() override
	{
		ImGui::Begin("Statistics");

		ImVec4 textCol = FPS < 20.0f ? ImVec4(1.0f, 0.0f, 0.0f, 1.0f) : FPS < 40.0f ? ImVec4(1.0f, 1.0f, 0.0f, 1.0f) : ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
		ImGui::TextColored(textCol, "FPS: %.0f", FPS);
		ImGui::TextColored(textCol, "MS: %.2f", MS);
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport");

		ImVec2 screenSize = ImGui::GetContentRegionAvail();

		Walnut::Timer timer;
		renderer.Render(screenSize.x, screenSize.y, MS);
		MS = timer.ElapsedMillis();
		FPS = 1000.0f / MS;

		std::shared_ptr<Walnut::Image> image = renderer.GetFinalImage();
		if (image)
		{
			ImGui::Image(image->GetDescriptorSet(),
				ImVec2((float)image->GetWidth(), (float)image->GetHeight()),
				ImVec2(0, 1), ImVec2(1, 0));
		}

		ImGui::End();
		ImGui::PopStyleVar();

		if (Walnut::Input::IsKeyDown(Walnut::Key::Escape))
			Walnut::Application::Get().Close();
	}
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Rocken Raster";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<RockenRaster>();

	return app;
}