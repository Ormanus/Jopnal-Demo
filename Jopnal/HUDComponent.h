#ifndef HUDCOMPONENT_H
#define HUDCOMPONENT_H

#include <Jopnal/Jopnal.hpp>

class UIComponent : public jop::Component
{
public:

    UIComponent(jop::Object& objRef, const std::string textureUp)
		: jop::Component(objRef, 0)
	{
        init(textureUp);
	};

	UIComponent(const UIComponent& misRef, jop::Object& objRef)
		: jop::Component(objRef, 0)
	{
	};

	JOP_GENERIC_COMPONENT_CLONE(UIComponent);

    void init(const std::string textureUp)
	{
        auto o = getObject();

        o->addTag("HUD");
		o->createComponent<jop::GenericDrawable>(o->getScene().getRenderer());
		auto drawable1 = o->getComponent<jop::GenericDrawable>();
		drawable1->setRenderGroup(1);
        auto& material1 = jop::ResourceManager::getEmptyResource<jop::Material>(textureUp, jop::Material::Attribute::Default);
		material1.setMap(jop::Material::Map::Diffuse, jop::ResourceManager::getResource<jop::Texture2D>(textureUp, true));
		drawable1->setModel(jop::Model(jop::ResourceManager::getNamedResource<jop::RectangleMesh>("rectangle", 100.0f), material1));
	}
};

class Button : public UIComponent
{
public:
    Button(jop::Object& objRef, const std::string textureUp, const std::string textureDown)
		: UIComponent(objRef, textureUp)
	{
		m_hover = false;
        m_message = "";
        size = glm::vec2(100.f);
        initMaterials(textureUp, textureDown);
	}

    void setMessage(int i, std::string m)
	{
        m_messageType = i;
		m_message = m;
	}

    void initMaterials(std::string TextureUp, std::string TextureDown)
    {
        auto& material1 = jop::ResourceManager::getEmptyResource<jop::Material>(TextureUp, jop::Material::Attribute::Default);
        material1.setMap(jop::Material::Map::Diffuse, jop::ResourceManager::getResource<jop::Texture2D>(TextureUp, true));

        auto& material2 = jop::ResourceManager::getEmptyResource<jop::Material>(TextureDown, jop::Material::Attribute::Default);
        material2.setMap(jop::Material::Map::Diffuse, jop::ResourceManager::getResource<jop::Texture2D>(TextureDown, true));

        m_textureDown = TextureDown;
        m_textureUp = TextureUp;

    }

    bool click();

    void setPressed(bool pressed){ m_pressed = pressed; }

	void mouseMove(const float mouseX, const float mouseY)
	{
        glm::vec2 windowSize = jop::Engine::getSubsystem<jop::Window>()->getSize();
        glm::vec3 pos = getObject()->getLocalPosition();
        pos.y = -pos.y;
        pos += glm::vec3(windowSize / 2.0f, 0.0f);
        glm::vec3 scale = getObject()->getGlobalScale();
        glm::vec2 sizePerTwo = size / 2.0f;
        glm::vec2 mousePos = jop::Engine::getSubsystem<jop::Window>()->getEventHandler()->getCursorPosition();

        mousePos = glm::vec2(mouseX, mouseY);

        if (mousePos.x > pos.x - sizePerTwo.x && mousePos.y > pos.y - sizePerTwo.y && mousePos.x < pos.x + sizePerTwo.x && mousePos.y < pos.y + sizePerTwo.y)
		{
            if (!m_hover)
            {
                getObject()->getComponent<jop::GenericDrawable>()->setModel(jop::Model(jop::ResourceManager::getNamedResource<jop::RectangleMesh>("rectangle", 100.0f), jop::ResourceManager::getExistingResource<jop::Material>(m_textureDown)));
            }

			m_hover = true;
		}
		else
		{
            if (m_hover)
            {
                getObject()->getComponent<jop::GenericDrawable>()->setModel(jop::Model(jop::ResourceManager::getNamedResource<jop::RectangleMesh>("rectangle", 100.0f), jop::ResourceManager::getExistingResource<jop::Material>(m_textureUp)));
            }
			m_hover = false;
		}
	}
private:
    std::string m_textureUp;
    std::string m_textureDown;

    int m_messageType;
	std::string m_message;
	glm::vec2 size;
	bool m_hover;
    bool m_pressed;
};

#endif