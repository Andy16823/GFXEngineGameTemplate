#include "MyGame.h"
#include "GFXEngine/Renderer.h"
#include "GFXEngine/PresentPipeline.h"
#include "GFXEngine/EngineDefinitions.h"
#include "GFXEngine/MeshBuilder.h"
#include "GFXEngine/MeshAsset.h"
#include "GFXEngine/MaterialAsset.h"
#include "GFXEngine/SolidMaterial.h"
#include "GFXEngine/Primitive.h"

using namespace GFXEngine;
using namespace GFXEngine::Core;
using namespace GFXEngine::Graphics;

/// <summary>
/// Loads the necessary assets for the game, including a primitive mesh and a solid material. 
/// This function is called before the game starts and is used to load all required assets into the asset manager.
/// </summary>
/// <param name="assetManager"></param>
void MyGame::loadAssets(GFXEngine::AssetManager& assetManager)
{
	MeshBuilder builder;

	auto cubeMesh = std::make_unique<Mesh3D>(builder.createCube().buildMesh3D());
	auto cubeMeshAsset = std::make_unique<MeshAsset>("PrimitiveMesh", std::move(cubeMesh));
	assetManager.addAsset(std::move(cubeMeshAsset));

	auto sphereMesh = std::make_unique<Mesh3D>(builder.createSphere(1.0f, 32, 32).buildMesh3D());
	auto sphereMeshAsset = std::make_unique<MeshAsset>("SphereMesh", std::move(sphereMesh));
	assetManager.addAsset(std::move(sphereMeshAsset));

	auto cylinderMesh = std::make_unique<Mesh3D>(builder.createCylinder(1.0f, 2.0f, 32).buildMesh3D());
	auto cylinderMeshAsset = std::make_unique<MeshAsset>("CylinderMesh", std::move(cylinderMesh));
	assetManager.addAsset(std::move(cylinderMeshAsset));

	auto primitiveMaterial = std::make_unique<SolidMaterial>();
	auto materialAsset = std::make_unique<MaterialAsset>("PrimitiveMaterial", std::move(primitiveMaterial));
	assetManager.addAsset(std::move(materialAsset));
}

/// <summary>
/// Initializes the game, setting up the camera, render texture, scene, and UI context. 
/// This function is called once when the game starts and is responsible for 
/// creating all necessary resources and initializing the game state.
/// </summary>
/// <param name="renderer"></param>
void MyGame::onInit(Graphics::Renderer& renderer)
{
	auto size = this->getWindowSize();
	uint32_t width = size.x;
	uint32_t height = size.y;

	m_camera = std::make_unique<Camera3D>(glm::vec3(0.0f, 35.0f, 0.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);
	m_camera->getTransform().rotateWorld(-35.0f, 45.0f, 0.0f);
	m_camera->createDescriptorSets(renderer, renderer.getUniformBufferLayout());
	for (uint32_t i = 0; i < renderer.getSwapchainImageCount(); i++)
	{
		m_camera->updateCameraBuffers(renderer, i);
	}

	m_renderTexture = std::make_unique<RenderTexture>();
	m_renderTexture->create(renderer, { width, height }, renderer.getOffscreenRenderPass());
	m_renderTexture->createDescriptorSet(renderer, 0, renderer.getSamplerLayout());

	m_scene = std::make_unique<Scene3D>();
	m_scene->directionalLight.intensity = 8.0f;
	m_scene->directionalLight.color = glm::vec3(0.88f, 0.64f, 1.00f);

	m_uiContext = std::make_unique<UIContext>();
	m_uiContext->init(renderer, this->getWindow());

	AssetHandle cubeMeshHandle = this->assetManager->get<MeshAsset>("PrimitiveMesh");
	AssetHandle sphereMeshHandle = this->assetManager->get<MeshAsset>("SphereMesh");
	AssetHandle cylinderMeshHandle = this->assetManager->get<MeshAsset>("CylinderMesh");
	AssetHandle material = this->assetManager->get<MaterialAsset>("PrimitiveMaterial");

	auto cube = std::make_unique<Primitive>(cubeMeshHandle, material, Defintions::SOLID_COLOR_PIPELINE);
	cube->setName("Cube");
	cube->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	cube->setScale(glm::vec3(1.0f));
	cube->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	m_scene->addEntity(std::move(cube));

	auto sphere = std::make_unique<Primitive>(sphereMeshHandle, material, Defintions::SOLID_COLOR_PIPELINE);
	sphere->setName("Sphere");
	sphere->setPosition(glm::vec3(3.0f, 0.0f, 0.0f));
	sphere->setScale(glm::vec3(1.0f));
	sphere->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	m_scene->addEntity(std::move(sphere));

	auto cylinder = std::make_unique<Primitive>(cylinderMeshHandle, material, Defintions::SOLID_COLOR_PIPELINE);
	cylinder->setName("Cylinder");
	cylinder->setPosition(glm::vec3(-3.0f, 0.0f, 0.0f));
	cylinder->setScale(glm::vec3(1.0f));
	cylinder->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	m_scene->addEntity(std::move(cylinder));

	this->assetManager->initializeGraphicsAssets(renderer);
	m_scene->setUseParallelRendering(true);
	m_scene->init(renderer);

	m_worldEditor = std::make_unique<GFXEditor::WorldEditor>(m_scene.get(), this, PROJECT_PATH);
	m_worldEditor->init(*m_uiContext, renderer);
	m_worldEditor->setVisible(true);
}

/// <summary>
/// Called once after the game has been initialized and is ready to start. 
/// This function can be used to perform any additional setup or initialization 
/// that needs to happen after the game has been fully initialized.
/// </summary>
/// <param name="renderer"></param>
void MyGame::onStart(Graphics::Renderer & renderer)
{
	
}

/// <summary>
/// Called every frame to update the game state, including the camera and scene.
/// </summary>
/// <param name="renderer"></param>
/// <param name="imageIndex"></param>
/// <param name="deltaTime"></param>
void MyGame::onUpdate(Graphics::Renderer & renderer, uint32_t imageIndex, float deltaTime)
{
	auto window = this->getWindow();
	m_camera->updateCameraBuffers(renderer, imageIndex);
	m_scene->update(*m_camera, deltaTime);

	if (m_worldEditor->isVisible()) 
	{
		m_worldEditor->update(*m_uiContext, *this->inputManager, deltaTime);
	}
}

/// <summary>
/// Called before the rendering of the scene begins. 
/// This function can be used to perform any necessary updates 
/// or preparations before rendering, such as updating 
/// uniform buffers or preparing the scene for rendering.
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
/// Called to render the scene and UI. This function handles rendering the 3D scene to an offscreen render texture,
/// </summary>
/// <param name="renderer"></param>
/// <param name="frameContext"></param>
/// <param name="imageIndex"></param>
void MyGame::onRender(Graphics::Renderer & renderer, GFXEngine::Graphics::FrameContext& frameContext, uint32_t imageIndex)
{
	// Render the scene to the offscreen render texture first.
	renderer.beginRenderPass(renderer.getOffscreenRenderPass(), m_renderTexture->getFramebuffer(), imageIndex);
	m_scene->render(renderer, *m_camera, frameContext, imageIndex);
	renderer.endRenderPass(imageIndex);

	// If the world editor is visible, we render its scene to the same render texture.
	if (m_worldEditor->isVisible()) {
		m_worldEditor->renderSceneToTexture(renderer, frameContext, m_uiContext.get(), imageIndex);
	}

	// Now we can render the final image to the screen using the present pipeline.
	auto presentPipeline = renderer.getPipeline<GFXEngine::Graphics::PresentPipeline>(Defintions::PipelineType::PRESENT_PIPELINE);
	renderer.beginRenderPass(renderer.getRenderPass(), imageIndex);
	renderer.usePipeline(*presentPipeline, imageIndex);
	m_renderTexture->draw(renderer, *presentPipeline, imageIndex);

	// Render the UI on top of the scene.
	m_uiContext->newFrame();
	if (m_worldEditor->isVisible()) {
		m_worldEditor->render(*m_uiContext, renderer, imageIndex);
	}
	m_uiContext->render(renderer, imageIndex);

	// End the render pass to finalize rendering for this frame.
	renderer.endRenderPass(imageIndex);
}

/// <summary>
/// Called after the rendering of the scene and UI is complete. 
/// This function can be used to perform any necessary cleanup or post-processing after rendering.
/// </summary>
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
/// Called when the game is being destroyed, allowing for cleanup of resources and disposal of assets.
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
/// Called when the swapchain is recreated, allowing for any necessary updates or adjustments to be made before the new swapchain is used.
/// </summary>
/// <param name="renderer"></param>
void MyGame::onSwapchainRecreate(Graphics::Renderer& renderer)
{
}

/// <summary>
/// Called after the swapchain has been recreated, allowing for any necessary updates or adjustments to be made to the render texture and its descriptor set.
/// </summary>
/// <param name="renderer"></param>
/// <param name="viewport"></param>
/// <param name="scissor"></param>
void MyGame::afterSwapchainRecreate(Graphics::Renderer& renderer, VkViewport viewport, VkRect2D scissor)
{
	m_renderTexture->resize(renderer, { static_cast<uint32_t>(viewport.width), static_cast<uint32_t>(viewport.height) }, renderer.getOffscreenRenderPass());
	m_renderTexture->updateDescriptorSet(renderer, 0, renderer.getSamplerLayout());
}

/// <summary>
/// Called when input is received, allowing for handling of keyboard input events.
/// </summary>
/// <param name="key"></param>
/// <param name="scancode"></param>
/// <param name="action"></param>
/// <param name="mods"></param>
void MyGame::onInput(int key, int scancode, int action, int mods)
{
	if (m_worldEditor->isVisible()) 
	{
		m_worldEditor->handleInput(this->getWindow(), key, scancode, action, mods);
	}
}

/// <summary>
/// Called when mouse input is received, allowing for handling of mouse button events.
/// </summary>
/// <param name="button"></param>
/// <param name="action"></param>
/// <param name="mods"></param>
void MyGame::onMouseInput(int button, int action, int mods)
{
	if (m_worldEditor->isVisible())
	{
		m_worldEditor->handleMouseInput(this->getWindow(), button, action, mods);
	}
}

/// <summary>
/// Called when the mouse is moved, allowing for handling of mouse movement events.
/// </summary>
/// <param name="xpos"></param>
/// <param name="ypos"></param>
void MyGame::onMouseMove(double xpos, double ypos)
{
	if (m_worldEditor->isVisible())
	{
		m_worldEditor->handleMouseMove(this->getWindow(), xpos, ypos);
	}
}

/// <summary>
/// Called when the mouse wheel is scrolled, allowing for handling of scroll events.
/// </summary>
/// <param name="xoffset"></param>
/// <param name="yoffset"></param>
void MyGame::onScroll(double xoffset, double yoffset)
{}