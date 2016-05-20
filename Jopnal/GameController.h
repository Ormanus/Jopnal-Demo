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
    FF_TOWER,
};

//----- The Master Component -----
class GameController : public jop::Component
{
public:
	GameController(jop::Object& objRef)
		: jop::Component(objRef, 1),
		m_action(SELECT)
	{
		init();
	};

	GameController(const GameController& misRef, jop::Object& objRef)
		: jop::Component(objRef, 1)
	{
		init();
	};

	JOP_GENERIC_COMPONENT_CLONE(GameController);

    void init();

    void mouseLeft();

    void action(const int a, const std::string& value);

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