#ifndef HUDCOMPONENT_H
#define HUDCOMPONENT_H

#include <Jopnal/Jopnal.hpp>

class UIComponent : public jop::Component
{
public:

	UIComponent(jop::Object& objRef, const std::string texturePath)
		: jop::Component(objRef, "name")
	{
        init(texturePath);
	};

	UIComponent(const UIComponent& misRef, jop::Object& objRef)
		: jop::Component(objRef, "name")
	{
	};

	JOP_GENERIC_COMPONENT_CLONE(UIComponent);

	void init(std::string texturePath)
	{
        auto o = getObject();// ->getScene().findChild("orthoCam");

        o->addTag("HUD");
		o->createComponent<jop::GenericDrawable>(o->getScene().getRenderer());
		auto drawable1 = o->getComponent<jop::GenericDrawable>();
		drawable1->setRenderGroup(1);
		auto& material1 = jop::ResourceManager::getEmptyResource<jop::Material>(texturePath, jop::Material::Attribute::Default);
		material1.setMap(jop::Material::Map::Diffuse, jop::ResourceManager::getResource<jop::Texture2D>(texturePath, true));
		drawable1->setModel(jop::Model(jop::ResourceManager::getNamedResource<jop::RectangleMesh>("rectangle", 100.0f), material1));
	}
};

class Button : public UIComponent
{
public:
	Button(jop::Object& objRef, const std::string texturePath)
		: UIComponent(objRef, texturePath)
	{
		m_hover = false;
        m_message = "";
        size = glm::vec2(100.f);
	}

	void setMessage(std::string m)
	{
		m_message = m;
	}

    bool click();
	

	void mouseMove(const float mouseX, const float mouseY)
	{
        glm::vec2 windowSize = jop::Engine::getSubsystem<jop::Window>()->getSize();
		glm::vec3 pos = getObject()->getLocalPosition() + glm::vec3(windowSize / 2.0f, 0.0f);
        glm::vec3 scale = getObject()->getGlobalScale();
        glm::vec2 sizePerTwo = size / 2.0f;
        glm::vec2 mousePos = /*glm::vec2(mouseX, mouseY);*/ jop::Engine::getSubsystem<jop::Window>()->getEventHandler()->getCursorPosition();

        if (mousePos.x > pos.x - sizePerTwo.x && mousePos.y > pos.y - sizePerTwo.y && mousePos.x < pos.x + sizePerTwo.x && mousePos.y < pos.y + sizePerTwo.y)
		{
            if (!m_hover)
            {
                //TODO: change material
            }

			m_hover = true;
		}
		else
		{
            if (m_hover)
            {
                //TODO: change material
            }
			m_hover = false;
		}
	}
private:
	std::string m_message;
	glm::vec2 size;
	bool m_hover;
};

#endif