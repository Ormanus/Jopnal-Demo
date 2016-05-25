#include "HUDComponent.h"
#include "SceneGame.h"

bool Button::click()
{
    if (m_hover && m_message != "")
    {
		setPressed(true);
        m_controller->action(m_messageType, m_message);
    }
    return m_hover;
}