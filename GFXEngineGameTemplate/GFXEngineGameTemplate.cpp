/**
 * This is the main entry point for the GFXEngine Game Template. It initializes the game and starts the main game loop.
 * You can use this template as a starting point for your own game development with GFXEngine.
 */

#include "MyGame.h"

using namespace std;

int main()
{
	std::filesystem::path shaderDir = std::filesystem::current_path() / "shaders";

	MyGame game;
	game.start(1280, 720, shaderDir.string(), "GFXEngine Game Template", false, true);
	return 0;
}
