#include "HUDComponent.h"
#include "SceneGame.h"

bool Button::click()
{
    if (m_hover && m_message != "")
        //jop::Engine::sendMessage(m_message);
        getObject()->getScene().findChild("GC")->getComponent<GameController>()->setAction(2);
    return m_hover;
}