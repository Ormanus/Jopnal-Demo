#ifndef ENEMY_H
#define ENEMY_H

#include <Jopnal/Jopnal.hpp>

class SceneStart;


class Enemy : public jop::Component
{
public:
    Enemy(jop::Object& objRef)
        : jop::Component(objRef, 0)
    {
        init();
    };

    Enemy(const Enemy& misRef, jop::Object& objRef)
        : jop::Component(objRef, 0)
    {
        init();
    };

    JOP_GENERIC_COMPONENT_CLONE(Enemy);

    void init();

    void setPath(const std::vector<glm::vec3>& path);

    void update(const float dt)override;

	int getReward() { return m_reward; }

    float getHealth() { return m_health; }

    void setHealth(float hp);

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