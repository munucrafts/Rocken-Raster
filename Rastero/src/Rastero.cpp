#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
//#include "Renderer.h"
#include <Walnut/Timer.h>


class Rastero : public Walnut::Layer
{
private:


public:
	virtual void OnUIRender() override
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport Window");

		//width = ImGui::GetContentRegionAvail().x;
		//height = ImGui::GetContentRegionAvail().y;

		//std::shared_ptr<Walnut::Image> image = renderer.GetFinalImage();

		//if (image)
		//	ImGui::Image(image->GetDescriptorSet(), 
		//	{(float)image->GetWidth(), (float)image->GetHeight()}, 
		//	ImVec2(0, 1), ImVec2(1, 0));

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