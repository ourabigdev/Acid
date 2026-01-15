#include "MainApp.hpp"

#include <iostream>
#include <Files/Files.hpp>
#include <Graphics/Graphics.hpp>
#include <Resources/Resources.hpp>
#include <Scenes/Scenes.hpp>
#include <Uis/Uis.hpp>
#include "MainRenderer.hpp"

using namespace acid;

int main(int argc, char **argv) {
	using namespace testBase;

	auto engine = std::make_unique<Engine>(argv[0]);
	engine->SetApp(std::make_unique<MainApp>());

	auto exitCode = engine->Run();
	engine = nullptr;

	std::cout << "Exited with code " << exitCode << std::endl;
	std::cin.get();

	return exitCode;
}

namespace testBase
{
	MainApp::MainApp()
		: App("Acid Test Base", Version(0, 1, 0))
	{
	}

	MainApp::~MainApp()
	{
		Graphics::Get()->SetRenderer(nullptr);
		Scenes::Get()->SetScene(nullptr);
		Uis::Get()->GetCanvas().ClearChildren();
	}

	void MainApp::Start()
	{
		std::cout << "=== MainApp::Start() BEGIN ===" << std::endl;

		std::cout << "Adding window..." << std::endl;
		auto window = Windows::Get()->AddWindow(); // ← CREATE the window
		std::cout << "Window created: " << (void *)window << std::endl;

		std::cout << "Setting window title..." << std::endl;
		window->SetTitle("Acid Test Base");

		std::cout << "Setting window icons..." << std::endl;
		window->SetIcons({"Icons/Icon-16.png", "Icons/Icon-24.png", "Icons/Icon-32.png", "Icons/Icon-48.png", "Icons/Icon-64.png", "Icons/Icon-96.png", "Icons/Icon-128.png", "Icons/Icon-192.png",
			"Icons/Icon-256.png"});

		std::cout << "Setting up Graphics..." << std::endl;
		Graphics::Get()->SetRenderer(std::make_unique<MainRenderer>());

		std::cout << "=== MainApp::Start() END ===" << std::endl;
	}

	void MainApp::Update() {}
}