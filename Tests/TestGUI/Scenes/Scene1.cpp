#include "Scene1.hpp"

#include <Inputs/Inputs.hpp>
#include <Uis/Constraints/PixelConstraint.hpp>
#include <Uis/Constraints/RelativeConstraint.hpp>
#include <Uis/Drivers/ConstantDriver.hpp>
#include <Uis/Drivers/SlideDriver.hpp>
#include <Uis/Uis.hpp>

namespace test {
constexpr Time UI_SLIDE_TIME = 0.2s;

Scene1::Scene1()
	: Scene(std::make_unique<Camera>())
{
	uiStartLogo.SetAlphaDriver<ConstantDriver>(1.0f);
	uiStartLogo.OnFinished().connect([this]() { TogglePause(); });
	Uis::Get()->GetCanvas().AddChild(&uiStartLogo);

	uiPanels.SetAlphaDriver<ConstantDriver>(0.0f);
	Uis::Get()->GetCanvas().AddChild(&uiPanels);

	overlayDebug.GetConstraints().SetWidth<PixelConstraint>(100).SetHeight<PixelConstraint>(36).SetX<PixelConstraint>(0, UiAnchor::Left).SetY<PixelConstraint>(0, UiAnchor::Bottom);
	overlayDebug.SetAlphaDriver<ConstantDriver>(1.0f); 
	Uis::Get()->GetCanvas().AddChild(&overlayDebug); 

	Inputs::Get()->GetButton("pause")->OnButton().connect(
		[this](InputAction action, bitmask::bitmask<InputMod> mods)
		{
			if (action == InputAction::Press)
			{
				TogglePause();
			}
		});
}

void Scene1::Start() {
}

void Scene1::Update() {
	Scene::Update();
}

bool Scene1::IsPaused() const {
	return !uiStartLogo.IsFinished() || uiPanels.GetAlphaDriver()->Get() > 0.0f;
}

void Scene1::TogglePause() {
	if (!uiStartLogo.IsFinished()) {
		return;
	}

	uiPanels.SetAlphaDriver<SlideDriver>(uiPanels.GetAlphaDriver()->Get(), IsPaused() ? 0.0f : 1.0f, UI_SLIDE_TIME);
}
}
