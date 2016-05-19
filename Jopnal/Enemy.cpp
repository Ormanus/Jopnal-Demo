#include "Enemy.h"
#include "GameController.h"

void Enemy::setPath(const std::vector<glm::vec3>& path)
{
    m_path = path;
    int m_currentWaypoint = 0;
}

void Enemy::update(float dt)
{
    if (m_path.size() > 0)
    {
        //move
        jop::Object& o = getObject();
        glm::vec3 delta = -o.getGlobalPosition() + m_path[m_currentWaypoint];

        float dist = glm::length(delta);
        delta /= dist;

        if (dist < 1.f)
        {
            m_currentWaypoint++;
            if (m_currentWaypoint >= m_path.size())
            {
                //reduce lives
                getObject()->removeSelf();
                getObject()->getScene().findChild("GC")->getComponent<GameController>()->addLives(-1);
            }
        }
        else
        {
            o.move(delta * m_speed * dt);
        }
    }
}

float Enemy::getDistanceFromHome()
{
    //really unaccurate, but just enough for sorting
    float dist = m_currentWaypoint * 1000000000;
    glm::vec3 delta(0);
    if (m_currentWaypoint < m_path.size())
        delta = getObject()->getGlobalPosition() - m_path[m_currentWaypoint];
    dist += delta.x*delta.x + delta.y*delta.y;
    return dist;
}