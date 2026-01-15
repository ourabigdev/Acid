#pragma once

#include <Graphics/Renderer.hpp>

using namespace acid;

namespace testBase
{
	class MainRenderer : public Renderer
	{
	public:
		MainRenderer();

		void Start() override;
		void Update() override;

	private:
		bool initialized = false;
	};
}