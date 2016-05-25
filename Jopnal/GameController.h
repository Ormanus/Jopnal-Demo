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

class Controller : public jop::Component
{
public:
	Controller(jop::Object& objRef)
		: jop::Component(objRef, 1)
	{};

	virtual void init() = 0;

	virtual void mouseLeft() = 0;

	virtual void action(const int a, const std::string& value) = 0;

};

//  ----  GAME CONTROLLER  ----
class GameController : public Controller
{
public:
	GameController(jop::Object& objRef)
		: Controller(objRef),
		m_action(SELECT)
	{
		init();
	};

	GameController(const GameController& misRef, jop::Object& objRef)
		: Controller(objRef)
	{
		init();
	};

	JOP_GENERIC_COMPONENT_CLONE(GameController);

    void init()override;

    void mouseLeft()override;

    void action(const int a, const std::string& value)override;

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
    
	bool isGameOver(){ return gameOver; };

private:

    jop::Text& createText(std::string id, float x, float y, std::string text);

	//Interaction variables
	Action m_action;
	jop::WeakReference<jop::Object> m_selected;

	bool gameOver;

	//Game variables
	int m_score;
	int m_money;
	int m_lives;
	int m_level;
};

#endif