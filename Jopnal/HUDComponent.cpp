#include "HUDComponent.h"
#include "SceneGame.h"

bool Button::click()
{
    if (m_hover && m_message != "")
    {
        getObject()->getScene().findChild("GC")->getComponent<GameController>()->action(0, m_message);
        setPressed(true);
    }
    return m_hover;
}