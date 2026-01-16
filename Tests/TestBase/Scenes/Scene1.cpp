#include "Scene1.hpp"

#include <Animations/AnimatedMesh.hpp>
#include <Audio/Sound.hpp>
#include <Files/File.hpp>
#include <Files/File.hpp>
#include <Files/Json/Json.hpp>
#include <Gizmos/Gizmos.hpp>
#include <Graphics/Graphics.hpp>
#include <Inputs/Inputs.hpp>
#include <Lights/Light.hpp>
#include <Materials/DefaultMaterial.hpp>
#include <Meshes/Mesh.hpp>
#include <Models/Shapes/CubeModel.hpp>
#include <Models/Shapes/CylinderModel.hpp>
#include <Models/Shapes/SphereModel.hpp>
#include <Particles/Emitters/CircleEmitter.hpp>
#include <Particles/Particles.hpp>
#include <Particles/ParticleSystem.hpp>
#include <Physics/Colliders/CapsuleCollider.hpp>
#include <Physics/Colliders/ConeCollider.hpp>
#include <Physics/Colliders/ConvexHullCollider.hpp>
#include <Physics/Colliders/CubeCollider.hpp>
#include <Physics/Colliders/CylinderCollider.hpp>
#include <Physics/Colliders/HeightfieldCollider.hpp>
#include <Physics/Colliders/SphereCollider.hpp>
#include <Physics/Physics.hpp>
#include <Resources/Resources.hpp>
#include <Scenes/EntityPrefab.hpp>
#include <Scenes/Scenes.hpp>
#include <Shadows/ShadowRender.hpp>
#include <Skyboxes/SkyboxMaterial.hpp>
#include <Uis/Constraints/PixelConstraint.hpp>
#include <Uis/Constraints/RelativeConstraint.hpp>
#include <Uis/Drivers/ConstantDriver.hpp>
#include <Uis/Drivers/SlideDriver.hpp>
#include <Uis/Uis.hpp>
#include "Behaviours/HeightDespawn.hpp"
#include "Behaviours/NameTag.hpp"
#include "Behaviours/Rotate.hpp"
#include "Terrain/TerrainMaterial.hpp"
#include "Terrain/Terrain.hpp"
#include "World/World.hpp"
#include "FpsCamera.hpp"

namespace test {
constexpr Time UI_SLIDE_TIME = 0.2s;

Scene1::Scene1() :
	Scene(std::make_unique<FpsCamera>()) {
	AddSystem<World>();
	AddSystem<Physics>();
	AddSystem<Particles>();
	AddSystem<Gizmos>();
	
	//uiStartLogo.SetTransform({UiMargins::All});
	uiStartLogo.SetAlphaDriver<ConstantDriver>(1.0f);
	uiStartLogo.OnFinished().connect([this]() {
		overlayDebug.SetAlphaDriver<SlideDriver>(0.0f, 1.0f, UI_SLIDE_TIME);
		Windows::Get()->GetWindow(0)->SetCursorHidden(true);
	});
	Uis::Get()->GetCanvas().AddChild(&uiStartLogo);

	//overlayDebug.SetTransform({{100, 36}, UiAnchor::LeftBottom});
	overlayDebug.GetConstraints().SetWidth<PixelConstraint>(100)
		.SetHeight<PixelConstraint>(36)
		.SetX<PixelConstraint>(0, UiAnchor::Left)
		.SetY<PixelConstraint>(0, UiAnchor::Bottom);
	overlayDebug.SetAlphaDriver<ConstantDriver>(0.0f);
	Uis::Get()->GetCanvas().AddChild(&overlayDebug);

	Inputs::Get()->GetButton("captureMouse")->OnButton().connect([this](InputAction action, bitmask::bitmask<InputMod> mods) {
		if (action == InputAction::Press) {
			Windows::Get()->GetWindow(0)->SetCursorHidden(!Windows::Get()->GetWindow(0)->IsCursorHidden());
		}
	});

	Inputs::Get()->GetButton("save")->OnButton().connect([this](InputAction action, bitmask::bitmask<InputMod> mods) {
		if (action == InputAction::Press) {
			Resources::Get()->GetThreadPool().Enqueue([this]() {
				File sceneFile("Scene1.json", std::make_unique<Json>());

				auto entitiesNode = sceneFile.GetNode()["entities"];

				for (auto &entity : QueryAllEntities()) {
					auto &entityNode = entitiesNode->AddProperty();

					if (!entity->GetName().empty()) {
						entityNode["name"] = entity->GetName();
					}

					for (auto &component : entity->GetComponents()) {
						if (auto componentName = component->GetTypeName(); !componentName.empty()) {
							entityNode[componentName].Set(component);
						}
					}
				}

				sceneFile.Write(NodeFormat::Beautified);
			});
		}
	});

	Windows::Get()->OnMonitorConnect().connect([](Monitor *monitor, bool connected) {
		Log::Out("Monitor ", std::quoted(monitor->GetName()), " action: ", connected, '\n');
	});
	Windows::Get()->GetWindow(0)->OnClose().connect([]() {
		Log::Out("Window has closed!\n");
	});
	Windows::Get()->GetWindow(0)->OnIconify().connect([](bool iconified) {
		Log::Out("Iconified: ", iconified, '\n');
	});
	Windows::Get()->GetWindow(0)->OnDrop().connect([](std::vector<std::string> paths) {
		for (const auto &path : paths) {
			Log::Out("File dropped on window: ", path, '\n');
		}
	});
}

void Scene1::Start() {
	GetSystem<Physics>()->SetGravity({0.0f, -9.81f, 0.0f});
	GetSystem<Physics>()->SetAirDensity(1.0f);

	auto player = CreatePrefabEntity("Objects/Player/Player.json");
	player->AddComponent<Transform>(Vector3f(0.0f, 2.0f, 0.0f), Vector3f(0.0f, Maths::Radians(180.0f), 0.0f));

	auto skybox = CreatePrefabEntity("Objects/SkyboxClouds/SkyboxClouds.json");
	skybox->AddComponent<Transform>(Vector3f(), Vector3f(), Vector3f(2048.0f));

	//auto animated = CreateEntity("Objects/Animated/Animated.json");
	//animated->AddComponent<Transform>(Vector3f(5.0f, 0.0f, 0.0f), Vector3f(), Vector3f(0.3f));

	auto animated = CreateEntity();
	animated->AddComponent<Transform>(Vector3f(5.0f, 0.0f, 0.0f), Vector3f(), Vector3f(0.3f));
	animated->AddComponent<AnimatedMesh>("Objects/Animated/Model.dae", 
		std::make_unique<DefaultMaterial>(Colour::White, Image2d::Create("Objects/Animated/Diffuse.png"), 0.7f, 0.6f));
	animated->AddComponent<Rigidbody>(std::make_unique<CapsuleCollider>(3.0f, 6.0f, Transform(Vector3(0.0f, 2.5f, 0.0f))), 0.0f);
	animated->AddComponent<ShadowRender>();

#ifdef ACID_DEBUG
	EntityPrefab prefabAnimated("Prefabs/Animated.json");
	prefabAnimated << *animated;
	prefabAnimated.Write(NodeFormat::Beautified);
#endif

	auto sun = CreateEntity();
	sun->AddComponent<Transform>(Vector3f(1000.0f, 5000.0f, -4000.0f), Vector3f(0,0,50.0f), Vector3f(18.0f));
	auto light = sun->AddComponent<Light>(Colour::White*4);
	
	auto plane = CreateEntity();
	plane->AddComponent<Transform>(Vector3f(0.0f, -0.5f, 0.0f), Vector3f(), Vector3f(50.0f, 1.0f, 50.0f));
	plane->AddComponent<Mesh>(CubeModel::Create({1.0f, 1.0f, 1.0f}),
		std::make_unique<DefaultMaterial>(Colour::White, Image2d::Create("Undefined2.png", VK_FILTER_NEAREST)));
	plane->AddComponent<Rigidbody>(std::make_unique<CubeCollider>(Vector3f(1.0f, 1.0f, 1.0f)), 0.0f, 0.5f);
	plane->AddComponent<ShadowRender>();


	static const std::vector cubeColours = {Colour::Red, Colour::Lime, Colour::Yellow, Colour::Blue, Colour::Purple, Colour::Grey, Colour::White};

	for (int32_t i = 0; i < 5; i++) {
		for (int32_t j = 0; j < 5; j++) {
			auto cube = CreateEntity();
			cube->AddComponent<Transform>(Vector3f(static_cast<float>(i), static_cast<float>(j) + 0.5f, -10.0f));
			cube->AddComponent<Mesh>(CubeModel::Create({1.0f, 1.0f, 1.0f}), 
				std::make_unique<DefaultMaterial>(cubeColours[static_cast<uint32_t>(Maths::Random(0.0f, static_cast<float>(cubeColours.size())))], nullptr, 0.5f, 0.3f));
			cube->AddComponent<Rigidbody>(std::make_unique<CubeCollider>(), 0.5f, 0.3f);
			cube->AddComponent<ShadowRender>();
		}
	}



	auto cone = CreateEntity();
	cone->AddComponent<Transform>(Vector3f(-3.0f, 2.0f, 10.0f));
	cone->AddComponent<Mesh>(CylinderModel::Create(1.0f, 0.0f, 2.0f, 28, 2), 
		std::make_unique<DefaultMaterial>(Colour::Blue, nullptr, 0.0f, 1.0f));
	cone->AddComponent<Rigidbody>(std::make_unique<ConeCollider>(1.0f, 2.0f),
		/*std::make_unique<SphereCollider>(1.0f, Transform({0.0f, 2.0f, 0.0f})),*/ 1.5f);
	cone->AddComponent<ShadowRender>();
}

void Scene1::Update() {
	Scene::Update();
}

bool Scene1::IsPaused() const {
	//return !uiStartLogo.IsFinished();
	return false;
}
}
