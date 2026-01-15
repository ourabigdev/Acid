#include "MainRenderer.hpp"
#include <Graphics/Graphics.hpp>
#include <Graphics/Pipelines/PipelineGraphics.hpp>
#include <Maths/Vector3.hpp>
#include <Models/Model.hpp>


namespace testBase
{
	class BaseVertex
	{
	public:
		static Shader::VertexInput GetVertexInput(uint32_t baseBinding = 0)
		{ 
			std::vector<VkVertexInputBindingDescription> bindingDescription = {
				{baseBinding, sizeof(BaseVertex), VK_VERTEX_INPUT_RATE_VERTEX}
			};
			std::vector<VkVertexInputAttributeDescription> attributeDescription = {
				{0, baseBinding, VK_FORMAT_R32G32_SFLOAT, offsetof(BaseVertex, position)},
				{1, baseBinding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(BaseVertex, color)},
			};
			return {bindingDescription, attributeDescription};
		}
		Vector2f position;
		Vector3f color;
	};

	class BaseSubrender : public Subrender
	{
	public:
		explicit BaseSubrender(const Pipeline::Stage &stage):
			Subrender(stage), 
			pipeline(stage, {"Tutorial/Shaders/tri2.vert", "Tutorial/Shaders/tri2.frag"},
				{BaseVertex::GetVertexInput()}, {}, PipelineGraphics::Mode::Polygon, PipelineGraphics::Depth::None)
		{}

		void Render(const CommandBuffer &commandBuffer) override { 
			if (!model)
			{
				std::cout << "Warning: model is null in Render!" << std::endl;
				return;
			}
			pipeline.BindPipeline(commandBuffer);

			model->CmdRender(commandBuffer);
		}

		void SetModel(std::unique_ptr<Model> &&model) { this->model = std::move(model); }

	private:
		PipelineGraphics pipeline;
		std::unique_ptr<Model> model;
	};

	MainRenderer::MainRenderer() {
		std::cout << "MainRenderer constructor BEGIN" << std::endl;

		std::cout << "Creating attachments..." << std::endl;
		std::vector<Attachment> renderpassAttachements1{
			{0, "swapchain", Attachment::Type::Swapchain, false, VK_FORMAT_UNDEFINED, Colour::Aqua},
		};

		std::cout << "Creating subpasses..." << std::endl;
		std::vector<SubpassType> renderpassSubpasses1 = {{0, {0}}};

		std::cout << "Adding render stage..." << std::endl;
		AddRenderStage(std::make_unique<RenderStage>(renderpassAttachements1, renderpassSubpasses1));

		std::cout << "MainRenderer constructor END" << std::endl;
	}

	void MainRenderer::Start() {
	}

	void MainRenderer::Update()
	{
		
	}
}