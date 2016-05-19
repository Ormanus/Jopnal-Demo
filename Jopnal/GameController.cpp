#include "GameController.h"
#include "SceneStart.h"

jop::Text& GameController::createText(std::string id, float x, float y, std::string text)
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

void GameController::setLives(int amount)
{
    m_lives = amount;
    getObject()->findChild("Lives")->getComponent<jop::Text>()->setString("Lives: " + std::to_string(m_lives));

    if (m_lives < 0)
    {
        //game over!
        
    }
}

void GameController::mouseLeft()
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
        m_action = SELECT;
        JOP_DEBUG_INFO("Bullet tower created");
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
