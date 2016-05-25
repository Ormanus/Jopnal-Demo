#ifndef MENU_CONTROLLER
#define MENU_CONTROLLER

#include "GameController.h"

//  ----  MENU CONTROLLER  ----

class MenuController : public Controller
{
public:
	MenuController(jop::Object& objRef)
		: Controller(objRef)
	{
		init();
	};

	MenuController(const MenuController& misRef, jop::Object& objRef)
		: Controller(objRef)
	{
		init();
	};

	JOP_GENERIC_COMPONENT_CLONE(MenuController);

	void init()override;

	void mouseLeft()override;

	void action(const int a, const std::string& value)override;
};

#endif