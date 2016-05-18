#ifndef MISSILE_H
#define MISSILE_H

#include "MapComponent.h" //<Jopnal/Jopnal.hpp>

class Missile : public jop::Component
{
public:

    Missile(jop::Object& objRef)
        : jop::Component(objRef, "name"),
        m_timer(10.0f)
    {
        objRef.setScale(0.75f, 0.75f, 2.0f);
        objRef.setRotation(3.14159f / 2.f, 0.0f, 0.0f);
        m_speed = 10.f;
        m_tot_time = 0.0f;
		m_velocity = glm::vec3(0.f, 1.f, 0.f);
    };

    Missile(const Missile& misRef, jop::Object& objRef)
        : jop::Component(objRef, "name"),
        m_timer(10.0f)
    {
        objRef.setScale(0.75f, 0.75f, 2.0f);
        objRef.setRotation(3.14159f / 2.f, 0.0f, 0.0f);
        m_speed = 10.f;
        m_tot_time = 0.0f;
    };

    JOP_GENERIC_COMPONENT_CLONE(Missile);

    void setTarget(jop::WeakReference<jop::Object> o)
    {
        m_target = o;
    }

    void setVelocity(glm::vec3 vel)
    {
        m_velocity = vel;
    }

    void update(const float dt) override
    {
        auto o = getObject();
        if (!m_target.expired())
        {

            m_tot_time += dt;

			glm::vec3 vel = m_velocity;// glm::normalize(o->getLocalFront());
            glm::vec3 delta = m_target->getGlobalPosition() - o->getGlobalPosition();
            float l = glm::length(delta);
            delta = glm::normalize(delta);

            glm::vec3 axis = glm::cross(vel, delta); //axis

            glm::vec3 normal = glm::normalize(glm::cross(axis, vel)) * dt * 10.f; //turning direction

			glm::vec3 newVel = vel + normal;

			m_velocity = glm::normalize(newVel);
			
            float cosTheta = glm::dot(vel, delta);

            if (abs(acos(cosTheta)) < 0.2)
            {
                m_speed += dt;
            }
            else
            {
				if (m_speed > 2.5f)
				{
					m_speed -= dt*2.f;
				}
            }
            if (l < 1.0f)
            {
                m_target->removeSelf();
                o->removeSelf();
            }
        }
        else
        {
            o->rotate(dt * 2.0f, dt * 2.0f, dt * 2.0f);
            //getObject()->removeSelf();
        }
        o->move(m_velocity * 20.f * dt * m_speed);
        m_timer -= dt;
        if (m_timer < 0)
        {
            getObject()->removeSelf();
        }
    }
private:
    jop::WeakReference<jop::Object> m_target;
    glm::vec3 m_velocity;
    float m_timer;
    float m_speed;
    float m_tot_time;
};

class Bullet : public jop::Component
{
public:

    Bullet(jop::Object& objRef)
        : jop::Component(objRef, "name"),
        m_velocity(glm::vec3(0)),
        timer(5.0f)
    {};

    Bullet(const Bullet& bulRef, jop::Object& objRef)
        : jop::Component(objRef, "name"),
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