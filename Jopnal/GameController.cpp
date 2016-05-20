#include "GameController.h"
#include "SceneStart.h"

void GameController::init()
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

void GameController::action(const int type, const std::string& value)
{
    switch (type)
    {
    case 0:
    {
        Action a = static_cast<Action>(std::stoi(value));
        if (m_action == a)
        {
            m_action = SELECT;
        }
        else
        {
            m_action = a;
        }
        break;
    }
    }
    
}

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

        if (m_money >= 50)
        {
            addMoney(-50);
            if (jop::Engine::getCurrentScene().findChildrenWithTag("sword", false).empty())
            {
                JOP_DEBUG_INFO("404 Sword not found.");
                return;
            }

            auto& sword = *jop::Engine::getCurrentScene().findChildrenWithTag("sword", false)[0];
            glm::vec3 pos = sword.getLocalPosition();

            //create tower
            auto tower = jop::Engine::getCurrentScene().createChild("bullet_tower");
            tower->setPosition(pos + glm::vec3(0.f, 1.f, 0.f));
            tower->createComponent<BulletTower>();

            JOP_DEBUG_INFO("Bullet tower Created.");
        }
        //else, show message "Not enough money!"
        //"purchase more in-game money from the website..."
        action(0, "0");
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
            tower->createComponent<MissileTower>();

            JOP_DEBUG_INFO("Tower Created.");
        }
        //else, show message "Not enough money!"
        //"purchase more in-game money from the website..."
        action(0, "0");
        break;
    }
    case FF_TOWER:
        if (m_money >= 200)
        {
            addMoney(-200);
            if (jop::Engine::getCurrentScene().findChildrenWithTag("sword", false).empty())
            {
                JOP_DEBUG_INFO("404 Sword not found.");
                return;
            }

            auto& sword = *jop::Engine::getCurrentScene().findChildrenWithTag("sword", false)[0];
            glm::vec3 pos = sword.getLocalPosition();

            //create tower
            auto tower = jop::Engine::getCurrentScene().createChild("ff_tower");
            tower->setPosition(pos + glm::vec3(0.f, 1.f, 0.f));
            tower->createComponent<FFTower>();

            JOP_DEBUG_INFO("Forcefield Created.");
        }
        action(0, "0");
        break;
    default:
        break;
    }
}
