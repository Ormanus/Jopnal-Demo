#ifndef SCENE_START_H
#define SCENE_START_H

#define PI 3.14159265359

#include <Jopnal/Jopnal.hpp>
#include "Ammo.h"
#include "HUDComponent.h"
#include "MenuController.h"

class MenuEventHandler
    : public jop::WindowEventHandler
{
public:
	MenuEventHandler(jop::Window& w, jop::Window& w2)
        :jop::WindowEventHandler(w)
    {
    }
	~MenuEventHandler(){}

    void keyPressed(const int key, const int, const int)override
    {
        if (key == jop::Keyboard::Escape)
        {
            closed();
        }
        if (key == jop::Keyboard::Right)
        {
        }
        if (key == jop::Keyboard::Left)
        {
        }
    }

    void mouseMoved(const float x, const float y) override
    {
        if (!jop::Engine::exiting() && jop::Engine::hasCurrentScene())
        {
			for (auto o : jop::Engine::getCurrentScene().findChildrenWithTag("HUD", true))
			{
				auto c = o->getComponent<Button>();
				if (c != nullptr)
				{
					c->mouseMove(x, y);
				}
			}
        }
    }

	void mouseButtonReleased(const int button, const int)override
	{
		if (button == jop::Mouse::Button::Left)
		{
			if (!jop::Engine::exiting() && jop::Engine::hasCurrentScene())
			{
				jop::Engine::getCurrentScene().findChild("GC")->getComponent<MenuController>()->mouseLeft();
			}
		}
	}

    void closed()override
    {
        jop::Engine::exit();
    }
private:

};

class SceneStart : public jop::Scene
{
private:
    std::vector<jop::WeakReference<jop::Object>> m_rings;
    float m_time;

public:
	SceneStart();

	void postUpdate(const float dt) override;

private:
	Button* createButton(glm::vec2 position, glm::vec2 size, std::string text, std::string path1, std::string path2, std::string path3)
	{
		auto gc = findChild("GC")->getComponent<MenuController>();

		auto o = findChild("orthoCam")->createChild("button");
		o->createComponent<Button>(path1, path2, path3, gc);
		o->setPosition(glm::vec3(position.x, position.y, 0.0f)).setScale(glm::vec3(size.x, size.y, 1.0f));
		
		auto o2 = o->createChild("Text");
		o2->setScale(-800.0f, -800.0f, 1.0f).setPosition(position.x * 800.f + 32.f, position.y * 10.f, 1.f);
		jop::Text& textComponent = o2->createComponent<jop::Text>(getRenderer());
		textComponent.setFont(jop::ResourceManager::getResource<jop::Font, std::string, int>(std::string("fonts/comic.ttf"), 128));
		textComponent.setRenderGroup(1);
		textComponent.setString(text);
		textComponent.setColor(jop::Color(0.0f, 1.0f, 1.0f, 1.0f));

		return o->getComponent<Button>();
	}
};

#endif