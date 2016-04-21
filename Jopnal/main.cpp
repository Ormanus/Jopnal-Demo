#include <Jopnal/Jopnal.hpp>
#include <Jopnal/Graphics.hpp>
#include <vector>
#include "SceneStart.h"
#include "SceneGame.h"

int main(int c, char** v)
{
    JOP_ENGINE_INIT("NO_NAME", c, v);

    jop::Engine::createScene<SceneGame>();

	return JOP_MAIN_LOOP;
}