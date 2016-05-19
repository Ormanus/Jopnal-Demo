#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

#include <Jopnal\Jopnal.hpp>
#include "HUDComponent.h"
#include "Tower.h"

enum Action
{
	SELECT,
	BULLET_TOWER,
	MISSILE_TOWER,
};

//----- The Master Component -----
class GameController : public jop::Component
{
public:
	GameController(jop::Object& objRef)
		: jop::Component(objRef, "GC"),
		m_action(SELECT)
	{
		init();
	};

	GameController(const GameController& misRef, jop::Object& objRef)
		: jop::Component(objRef, "GC")
	{
		init();
	};

	JOP_GENERIC_COMPONENT_CLONE(GameController);

	void init()
	{
		//HUD
		auto o = getObject();

		glm::vec2 winsize = jop::Engine::getSubsystem<jop::Window>()->getSize();

		createText("Money", winsize.x / 2.f - 200.f, winsize.y / 2.f - 50.f, "Money: ");
		createText("Lives", winsize.x / 2.f - 200.f, winsize.y / 2.f - 100.f, "Lives: ");
		createText("Score", winsize.x / 2.f - 200.f, winsize.y / 2.f - 150.f, "Score: ");

		setMoney(500);
		setScore(0);
        setLives(10);
	}

    void mouseLeft();

	void setAction(int a)
	{
		if (m_action == a)
		{
			m_action = SELECT;
		}
		else
		{
			m_action = static_cast<Action>(a);
		}
	}

	int getMoney()
	{
		return m_money;
	}

	void addMoney(int amount)
	{
		setMoney(m_money + amount);
	}

	void setMoney(int amount)
	{
		m_money = amount;
		getObject()->findChild("Money")->getComponent<jop::Text>()->setString("Money: " + std::to_string(m_money));
	}

	void addScore(int amount)
	{
		setMoney(m_score + amount);
	}

	void setScore(int amount)
	{
		m_score = amount;
		getObject()->findChild("Score")->getComponent<jop::Text>()->setString("Score: " + std::to_string(m_score));
	}

    void addLives(int amount)
    {
        setLives(m_lives + amount);
    }

    void setLives(int amount);
    
private:

    jop::Text& createText(std::string id, float x, float y, std::string text);

	//Interaction variables
	Action m_action;
	jop::WeakReference<jop::Object> m_selected;

	//Game variables
	int m_score;
	int m_money;
	int m_lives;
	int m_level;
};

#endif