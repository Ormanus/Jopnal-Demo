#include "Enemy.h"
#include "GameController.h"

void Enemy::init()
{
    auto o = getObject();
    o->addTag("target");

    //drawable
    auto& drawable = o->createComponent<jop::GenericDrawable>(getObject()->getScene().getRenderer());
    drawable.setModel(jop::Model(jop::Mesh::getDefault(), jop::ResourceManager::getExistingResource<jop::Material>("cubeMaterial")));

    setHealth(100.f);
    m_speed = 8.f;
    m_currentWaypoint = 0;

    m_reward = 10;
    //m_maxWaypoint = -1;
}

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

void Enemy::setHealth(float hp)
{ 
    m_health = hp; 
    if (m_health < 0.f)
    {
        getObject()->getScene().findChild("GC")->getComponent<GameController>()->addMoney(getReward());
		getObject()->getScene().findChild("GC")->getComponent<GameController>()->addScore(getReward());
        getObject()->removeSelf();
    }

    getObject()->setScale((m_health * 2.f / 100.f) + 0.25f);
}