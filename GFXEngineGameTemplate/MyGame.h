#pragma once
#include "GFXEngine/Game.h"
#include "GFXEngine/Camera.h"
#include "GFXEngine/RenderTexture.h"
#include "GFXEngine/Scene3D.h"
#include "GFXEngine/UIContext.h"
#include "GFXEditor/WorldEditor.h"

class MyGame : public GFXEngine::Core::Game
{
private:
	std::unique_ptr<GFXEngine::Graphics::Camera3D> m_camera;
	std::unique_ptr<GFXEngine::Graphics::RenderTexture> m_renderTexture;
	std::unique_ptr<GFXEngine::Core::Scene3D> m_scene;
	std::unique_ptr<GFXEngine::Core::UIContext> m_uiContext;
	std::unique_ptr<GFXEditor::WorldEditor> m_worldEditor;

public:

	// Geerbt über Game
	void onInit(GFXEngine::Graphics::Renderer& renderer) override;
	void onStart(GFXEngine::Graphics::Renderer& renderer) override;
	void onUpdate(GFXEngine::Graphics::Renderer& renderer, uint32_t imageIndex, float deltaTime) override;
	void beforeRender(GFXEngine::Graphics::Renderer& renderer, uint32_t imageIndex) override;
	void onRender(GFXEngine::Graphics::Renderer& renderer, GFXEngine::Graphics::FrameContext& frameContext, uint32_t imageIndex) override;
	void afterRender(GFXEngine::Graphics::Renderer& renderer, uint32_t imageIndex) override;
	void onDestroy(GFXEngine::Graphics::Renderer& renderer) override;
	void onInput(int key, int scancode, int action, int mods) override;
	void onMouseInput(int button, int mods, int action) override;
	void onMouseMove(double xpos, double ypos) override;
	void onScroll(double xoffset, double yoffset) override;
	void onSwapchainRecreate(GFXEngine::Graphics::Renderer& renderer) override;
	void afterSwapchainRecreate(GFXEngine::Graphics::Renderer& renderer, VkViewport viewport, VkRect2D scissor) override;
};
