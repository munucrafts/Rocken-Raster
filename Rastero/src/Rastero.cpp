#include <Walnut/Application.h>
#include <Walnut/EntryPoint.h>
#include <Walnut/Timer.h>
#include <Walnut/Image.h>

#include "Renderer.h"

class Rastero : public Walnut::Layer
{
private:
	Renderer renderer;
	float MS;
	float FPS;

public:
	virtual void OnUIRender() override
	{
		ImGui::Begin("Stats");

		ImVec4 textCol = FPS < 20.0f ? ImVec4(1.0f, 0.0f, 0.0f, 1.0f) : FPS < 40.0f ? ImVec4(1.0f, 1.0f, 0.0f, 1.0f) : ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
		ImGui::TextColored(textCol, "FPS: %.0f", FPS);
		ImGui::TextColored(textCol, "MS: %.2f", MS);
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport Window");

		ImVec2 screenSize = ImGui::GetContentRegionAvail();

		Walnut::Timer timer;
		renderer.Render(screenSize.x, screenSize.y, MS * 1000.0f);
		MS = timer.ElapsedMillis();
		FPS = 1000.0f / MS;

		std::shared_ptr<Walnut::Image> image = renderer.GetImage();
		if (image)
		{
			ImGui::Image(image->GetDescriptorSet(),
				ImVec2((float)image->GetWidth(), (float)image->GetHeight()),
				ImVec2(0, 1), ImVec2(1, 0));
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Rastero";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<Rastero>();

	return app;
}