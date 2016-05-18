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
	}

	void mouseLeft()
	{
		switch (m_action)
		{
		case SELECT:
		{
			//try to click buttons

			for (auto object : getObject()->getScene().findChildrenWithTag("HUD", true))
			{
				auto button = object->getComponent<Button>();
				if (button != nullptr)
				{
					if (button->click())
					{
						//"collision" found, don't click other buttons or select towers
						return;
					}
				}
			}

			//else
			//TODO: select tower
			break;
		}
		case BULLET_TOWER:
		{
			break;
		}
		case MISSILE_TOWER:
		{
			if (m_money >= 100)
			{
				addMoney(-100);
				if (jop::Engine::getCurrentScene().findChildrenWithTag("sword", false).empty())
				{
					JOP_DEBUG_INFO("404 Sword not found.");
					return;
				}

				auto& sword = *jop::Engine::getCurrentScene().findChildrenWithTag("sword", false)[0];
				glm::vec3 pos = sword.getLocalPosition();

				//create tower
				auto tower = jop::Engine::getCurrentScene().createChild("tower");
				tower->setPosition(pos + glm::vec3(0.f, 1.f, 0.f));
				tower->createComponent<Tower>();
				auto& drawable = tower->createComponent<jop::GenericDrawable>(jop::Engine::getCurrentScene().getRenderer());
				drawable.setModel(jop::Model(jop::Mesh::getDefault(), jop::ResourceManager::getExistingResource<jop::Material>("cubeMaterial")));

				JOP_DEBUG_INFO("Tower Created.");
			}
			//else, show message "Not enough money!"
			//"purchase more in-game money from the website..."
			m_action = SELECT;
			break;
		}
		default:
			break;
		}
	}

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
private:

	jop::Text& createText(std::string id, float x, float y, std::string text)
	{
		auto textObject = getObject()->createChild(id);
		textObject->setScale(800.0f, 800.0f, 1.0f);
		textObject->setPosition(x, y, 0.0f);
		jop::Text& textComponent = textObject->createComponent<jop::Text>(getObject()->getScene().getRenderer());
		textComponent.setFont(jop::ResourceManager::getResource<jop::Font, std::string, int>(std::string("fonts/novem___.ttf"), 64));
		textComponent.setRenderGroup(1);
		textComponent.setString(text);
		textComponent.setColor(jop::Color(0.9f, 0.4f, 0.1f, 1.0f));

		return textComponent;
	}

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