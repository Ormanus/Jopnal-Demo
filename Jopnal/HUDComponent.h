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
        m_message = nullptr;
	}

	void setAction(jop::Message* m)
	{
		m_message = m;
	}

	void onClick()
	{
		if (m_hover && m_message != nullptr)
			jop::Engine::sendMessage(*(m_message));
	}

	void onMouseMove(float mouseX, float mouseY)
	{
		glm::vec3 pos = getObject()->getGlobalPosition();
		if (mouseX > pos.x && mouseY > pos.y && mouseX < pos.x + size.x && mouseY < pos.y + size.y)
		{
			m_hover = true;
		}
		else
		{
			m_hover = false;
		}
	}
private:
	jop::Message* m_message;
	glm::vec2 size;
	bool m_hover;
};

#endif