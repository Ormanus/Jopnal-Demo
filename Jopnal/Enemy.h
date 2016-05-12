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
        m_speed = 10.f;
        m_currentWaypoint = 0;
        m_maxWaypoint = -1;
    };

    Enemy(const Enemy& misRef, jop::Object& objRef)
        : jop::Component(objRef, "name")
    {
    };

    JOP_GENERIC_COMPONENT_CLONE(Enemy);

    void setPath(glm::vec3* waypoints, int n_waypoints)
    {
        m_waypoints = waypoints;
        m_maxWaypoint = n_waypoints-1;
        int m_currentWaypoint = 0;
    }

    void update(float dt)override
    {
        if (m_maxWaypoint > -1)
        {
            //move
            jop::Object& o = getObject();
            glm::vec3 delta = -o.getGlobalPosition() + m_waypoints[m_currentWaypoint];

            float dist = glm::length(delta);
            delta /= dist;

            if (dist < 1.f)
            {
                m_currentWaypoint++;
                if (m_currentWaypoint > m_maxWaypoint)
                {
                    //game over
                }
            }
            else
            {
                o.move(delta * m_speed * dt);
            }
        }
    }
private:
    float m_health;
    float m_speed;
    glm::vec3* m_waypoints;
    int m_currentWaypoint;
    int m_maxWaypoint;
};

#endif