#pragma once
#include <Engine/App.hpp>
#include <Fonts/Text.hpp>

using namespace acid;
namespace testBase
{
	class MainApp : public App, public rocket::trackable
	{
	public:
		MainApp();
		~MainApp();

		void Start() override;
		void Update() override;

	private:
		Text helloWorldText;
	};
}