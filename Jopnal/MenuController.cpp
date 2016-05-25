#include "MenuController.h"
#include "SceneGame.h"

void MenuController::init()
{

}

void MenuController::mouseLeft()
{
	for (auto object : getObject()->getScene().findChildrenWithTag("HUD", true))
	{
		auto button = object->getComponent<Button>();
		if (button != nullptr)
		{
			if (button->click())
			{
				//"collision" found, don't click other buttons or select (or place!) towers
				return;
			}
		}
	}
}

void MenuController::action(const int a, const std::string& value)
{
	switch (a)
	{
	case 0:
		jop::Engine::createScene<SceneGame>();
		break;
	case 1:
		jop::Engine::exit();
		break;
	default:
		break;
	}
}