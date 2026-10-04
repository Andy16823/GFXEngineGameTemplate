#include "MyGame.h"
#include "GFXEngine/Renderer.h"
#include "GFXEngine/PresentPipeline.h"
#include "GFXEngine/EngineDefinitions.h"

using namespace GFXEngine;
using namespace GFXEngine::Core;
using namespace GFXEngine::Graphics;

/// <summary>
/// Within the onInit function, we set up the main camera, create the render texture for offscreen rendering, 
/// initialize the scene, and set up the UI context. This function is called once when the game starts and is 
/// used to initialize all necessary resources and state for the game.
/// </summary>
/// <param name="renderer"></param>
void MyGame::onInit(Graphics::Renderer& renderer)
{
	// Get the window size to set up the camera and render texture with the correct dimensions
	auto size = this->getWindowSize();
	uint32_t width = size.x;
	uint32_t height = size.y;

	// Create the main camera for the scene and update the camera buffers for each swapchain image
	m_camera = std::make_unique<Camera3D>(glm::vec3(0.0f, 35.0f, 0.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);
	m_camera->getTransform().rotateWorld(-35.0f, 45.0f, 0.0f);
	m_camera->createDescriptorSets(renderer, renderer.getUniformBufferLayout());
	for (uint32_t i = 0; i < renderer.getSwapchainImageCount(); i++)
	{
		m_camera->updateCameraBuffers(renderer, i);
	}

	// Create the render texture for offscreen rendering. This texture will be used to render the 3D scene before presenting it on the screen.
	m_renderTexture = std::make_unique<RenderTexture>();
	m_renderTexture->create(renderer, { width, height }, renderer.getOffscreenRenderPass());
	m_renderTexture->createDescriptorSet(renderer, 0, renderer.getSamplerLayout());

	// Create the main scene for the game. The scene will contain all the entities, lights, and other components that make up the 3D world.
	m_scene = std::make_unique<Scene3D>();
	m_scene->directionalLight.intensity = 8.0f;
	m_scene->directionalLight.color = glm::vec3(0.88f, 0.64f, 1.00f);

	// Create the UI context for rendering the user interface. The UI context will manage all the UI elements and render them on top of the 3D scene.
	m_uiContext = std::make_unique<UIContext>();
	m_uiContext->init(renderer, this->getWindow());

	// Enable parallel rendering for the scene to allow rendering of multiple objects in parallel, improving performance on multi-core systems.
	m_scene->setUseParallelRendering(true);
	m_scene->init(renderer); // Needs to called at the end of the onInit function to ensure all resources are initialized before the scene is set up

	// Initial world editor
	m_worldEditor = std::make_unique<GFXEditor::WorldEditor>(m_scene.get(), this, PROJECT_PATH);
	m_worldEditor->init(*m_uiContext, renderer);
	m_worldEditor->setVisible(true);
}

/// <summary>
/// Here we can set up any additional state or resources that depend on the renderer being fully initialized.
/// </summary>
/// <param name="renderer"></param>
void MyGame::onStart(Graphics::Renderer & renderer)
{
	
}

/// <summary>
/// Here we update the camera buffers with the current view and projection matrices for each swapchain image, 
/// and then update the scene with the current camera and delta time. 
/// This function is called every frame and is used to update the game state before rendering.
/// </summary>
/// <param name="renderer"></param>
/// <param name="imageIndex"></param>
/// <param name="deltaTime"></param>
void MyGame::onUpdate(Graphics::Renderer & renderer, uint32_t imageIndex, float deltaTime)
{
	// Get the current window and update the camera buffers with the current view and projection matrices
	auto window = this->getWindow();
	m_camera->updateCameraBuffers(renderer, imageIndex);
	m_scene->update(*m_camera, deltaTime);

	// Update world editor
	if (m_worldEditor->isVisible()) {
		m_worldEditor->update(*m_uiContext, *this->inputManager, deltaTime);
	}
}

/// <summary>
/// Before rendering, we can perform any necessary updates or state changes that need to happen before the scene is rendered.
/// </summary>
/// <param name="renderer"></param>
/// <param name="imageIndex"></param>
void MyGame::beforeRender(Graphics::Renderer & renderer, uint32_t imageIndex)
{
	m_scene->beforeRender(renderer, *m_camera, imageIndex);

	if (m_worldEditor->isVisible())
	{
		m_worldEditor->beforeRender(*m_uiContext, renderer, imageIndex);
	}
}

/// <summary>
/// Here we handle the main rendering logic for the game. This function is called every frame and is responsible for rendering the 3D scene, the UI, and any other visual elements.
/// </summary>
/// <param name="renderer"></param>
/// <param name="imageIndex"></param>
void MyGame::onRender(Graphics::Renderer & renderer, GFXEngine::Graphics::FrameContext& frameContext, uint32_t imageIndex)
{
	// First we render the 3D scene to the offscreen render texture.
	renderer.beginRenderPass(renderer.getOffscreenRenderPass(), m_renderTexture->getFramebuffer(), imageIndex);
	m_scene->render(renderer, *m_camera, frameContext, imageIndex);
	renderer.endRenderPass(imageIndex);

	// Then we render the contents of the render texture to the screen using the present pipeline. 
	// This allows us to apply post-processing effects or other screen-space effects if desired.
	auto presentPipeline = renderer.getPipeline<GFXEngine::Graphics::PresentPipeline>(Defintions::PipelineType::PRESENT_PIPELINE);
	renderer.beginRenderPass(renderer.getRenderPass(), imageIndex);
	renderer.usePipeline(*presentPipeline, imageIndex);
	m_renderTexture->draw(renderer, *presentPipeline, imageIndex);

	// Now we can render the UI on top of the 3D scene. The UI context will handle rendering all the UI elements and ensuring they are drawn in the correct order.
	m_uiContext->newFrame();
	if (m_worldEditor->isVisible()) {
		m_worldEditor->renderSceneToTexture(renderer, frameContext, m_uiContext.get(), imageIndex);
		m_worldEditor->render(*m_uiContext, renderer, imageIndex);
	}
	m_uiContext->render(renderer, imageIndex);

	// End the render pass to finalize rendering for this frame.
	renderer.endRenderPass(imageIndex);
}

/// <summary>
/// Here we can perform any necessary cleanup or state changes that need to happen after the scene has been rendered.
/// <param name="renderer"></param>
/// <param name="imageIndex"></param>
void MyGame::afterRender(Graphics::Renderer & renderer, uint32_t imageIndex)
{
	if (m_worldEditor->isVisible())
	{
		m_worldEditor->afterRender(*m_uiContext, renderer, imageIndex);
	}
}

/// <summary>
/// This function is called when the game is being destroyed and is used to clean up any resources that were created during the game's lifetime.
/// </summary>
/// <param name="renderer"></param>
void MyGame::onDestroy(Graphics::Renderer & renderer)
{
	m_worldEditor->dispose(*m_uiContext, renderer);
	m_uiContext->dispose(renderer);
	m_camera->destroyDescriptorSets(renderer);

	this->assetManager->destroyGraphicsAssets(renderer);

	m_scene->destroy(renderer);
	m_renderTexture->destroy(renderer);
}

/// <summary>
/// Since the engine uses Vulkan, we need to handle swapchain recreation when the window is resized or when the swapchain becomes invalid.
/// This is the callback BEFORE the swapchain is recreated, allowing us to clean up any resources that depend on the swapchain before it is destroyed.
/// Mainly you wont use this function, but it can be useful if you have resources that need to be explicitly cleaned up before the swapchain is recreated.
/// </summary>
/// <param name="renderer"></param>
void MyGame::onSwapchainRecreate(Graphics::Renderer& renderer)
{
}

/// <summary>
/// Since the engine uses Vulkan, we need to handle swapchain recreation when the window is resized or when the swapchain becomes invalid.
/// This is the callback AFTER the swapchain has been recreated, allowing us to recreate any resources that depend on the swapchain with the new dimensions and formats.
/// In this function, we resize the render texture to match the new swapchain dimensions and update its descriptor set to ensure it can be sampled correctly in the new render pass.
/// </summary>
/// <param name="renderer"></param>
/// <param name="viewport"></param>
/// <param name="scissor"></param>
void MyGame::afterSwapchainRecreate(Graphics::Renderer& renderer, VkViewport viewport, VkRect2D scissor)
{
	m_renderTexture->resize(renderer, { static_cast<uint32_t>(viewport.width), static_cast<uint32_t>(viewport.height) }, renderer.getOffscreenRenderPass());
	m_renderTexture->updateDescriptorSet(renderer, 0, renderer.getSamplerLayout());
}

void MyGame::onInput(int key, int scancode, int action, int mods)
{}

void MyGame::onMouseInput(int button, int mods, int action)
{
	if (m_worldEditor->isVisible())
	{
		m_worldEditor->handleMouseInput(this->getWindow(), button, action, mods);
	}
}

void MyGame::onMouseMove(double xpos, double ypos)
{
	if (m_worldEditor->isVisible())
	{
		m_worldEditor->handleMouseMove(this->getWindow(), xpos, ypos);
	}
}

void MyGame::onScroll(double xoffset, double yoffset)
{}