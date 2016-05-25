#include "SceneGame.h"

void SceneGame::postUpdate(const float dt)
{
	if (findChild("GC")->getComponent<GameController>()->isGameOver())
	{
		jop::Engine::createScene<SceneStart>();
	}
}