#ifndef MISSILE_H
#define MISSILE_H

#include "MapComponent.h" //<Jopnal/Jopnal.hpp>
//#include "GameController.h"

class GameController;

class Missile : public jop::Component
{
public:

    Missile(jop::Object& objRef)
        : jop::Component(objRef, "missile"),
        m_timer(10.0f)
    {
        objRef.setScale(0.75f, 0.75f, 2.0f);
        objRef.setRotation(3.14159f / 2.f, 0.0f, 0.0f);
        m_speed = 10.f;
        m_tot_time = 0.0f;
		m_velocity = glm::vec3(0.f, 1.f, 0.f);
    };

    Missile(const Missile& misRef, jop::Object& objRef)
        : jop::Component(objRef, "missile"),
        m_timer(10.0f)
    {
        objRef.setScale(0.75f, 0.75f, 2.0f);
        objRef.setRotation(3.14159f / 2.f, 0.0f, 0.0f);
        m_speed = 10.f;
        m_tot_time = 0.0f;
    };

    JOP_GENERIC_COMPONENT_CLONE(Missile);

	void setTarget(jop::WeakReference<jop::Object> o);

	void setVelocity(glm::vec3 vel);

	void update(const float dt) override;
private:
    jop::WeakReference<jop::Object> m_target;
    glm::vec3 m_velocity;
    float m_timer;
    float m_speed;
    float m_tot_time;
};

////  BULLET CLASS  ////

class Bullet : public jop::Component
{
public:

    Bullet(jop::Object& objRef)
        : jop::Component(objRef, "bullet"),
        m_velocity(glm::vec3(0)),
        timer(5.0f)
    {};

    Bullet(const Bullet& bulRef, jop::Object& objRef)
        : jop::Component(objRef, "bullet"),
        m_velocity(bulRef.getVelocity()),
        timer(5.0f)
    {};

    JOP_GENERIC_COMPONENT_CLONE(Bullet);

    void setVelocity(glm::vec3 vel)
    {
        m_velocity = vel;
    }

    const glm::vec3& getVelocity()const{ return m_velocity; }

    void update(const float dt) override
    {
        auto o = getObject();
        o->move(m_velocity * dt);
        timer -= dt;
        if (timer < 0)
        {
            getObject()->removeSelf();
        }
    }
private:
    glm::vec3 m_velocity;
    float timer;
};

#endif