#ifndef MISSILE_H
#define MISSILE_H

#include <Jopnal/Jopnal.hpp>

class Missile : public jop::Component
{
public:

    Missile(jop::Object& objRef)
        : jop::Component(objRef, "name"),
        m_timer(10.0f)
    {
        objRef.setScale(0.75f, 0.75f, 2.0f);
        objRef.setRotation(3.14159f / 2.f, 0.0f, 0.0f);
        m_speed = 1.f;
        m_tot_time = 0.0f;
    };

    Missile(const Missile& misRef, jop::Object& objRef)
        : jop::Component(objRef, "name"),
        m_timer(10.0f)
    {
        objRef.setScale(0.75f, 0.75f, 2.0f);
        objRef.setRotation(3.14159f / 2.f, 0.0f, 0.0f);
        m_speed = 1.f;
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

            glm::vec3 vel = glm::normalize(o->getLocalFront());
            glm::vec3 delta = m_target->getGlobalPosition() - o->getGlobalPosition();
            float l = glm::length(delta);
            delta = glm::normalize(delta);

            glm::vec3 axis = glm::cross(vel, delta);

            //glm::vec3 normal = glm::normalize(glm::cross(axis, vel));

            float cosTheta = glm::dot(vel, delta);

            float s = sqrt((1+cosTheta)*2);
            float invs = 1.f / s;

            glm::quat dir;

            dir = glm::quat(s * 0.5f, axis * invs);

            //dir = glm::slerp(glm::quat_cast(glm::mat3x3(1.f)), dir, m_tot_time);
            

            if (abs(acos(cosTheta)) < 0.1)
            {
                m_speed += dt;
            }
            else
            {
                dir = glm::slerp(glm::quat_cast(glm::mat3x3(1.f)), dir, 1.f);
            }

            o->rotate(dir);

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
        o->move(o->getGlobalFront() * 20.f * dt * m_speed);
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