#ifndef ENEMY_H
#define ENEMY_H

#include <Jopnal/Jopnal.hpp>

class SceneStart;


class Enemy : public jop::Component
{
public:
    Enemy(jop::Object& objRef)
        : jop::Component(objRef, "name")
    {
        auto o = getObject();
        o->createComponent<jop::GenericDrawable>(getObject()->getScene().getRenderer());
        o->addTag("target");
        auto drawable = o->getComponent<jop::GenericDrawable>();
        drawable->setModel(jop::Model(jop::Mesh::getDefault(), jop::ResourceManager::getExistingResource<jop::Material>("bulletMaterial")));

        m_health = 100.f;
        m_speed = 100.f;
        m_currentWaypoint = 0;

		m_reward = 10;
        //m_maxWaypoint = -1;
    };

    Enemy(const Enemy& misRef, jop::Object& objRef)
        : jop::Component(objRef, "name")
    {
    };

    JOP_GENERIC_COMPONENT_CLONE(Enemy);

    void setPath(const std::vector<glm::vec3>& path);

    void update(float dt)override;

	int getReward() { return m_reward; }

    float getHealth() { return m_health; }

    float getDistanceFromHome();

private:
    float m_health;
    float m_speed;
	std::vector<glm::vec3> m_path;
    int m_currentWaypoint;
	int m_reward;
    //int m_maxWaypoint;
};

#endif