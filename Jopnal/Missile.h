#ifndef MISSILE_H
#define MISSILE_H

#include <Jopnal/Jopnal.hpp>

class Missile : public jop::Component
{
public:

    Missile(jop::Object& objRef)
        : jop::Component(objRef, "name"),
        timer(30.0f)
    {
        objRef.setScale(0.75f, 0.75f, 2.0f);
    };

    Missile(const Missile& misRef, jop::Object& objRef)
        : jop::Component(objRef, "name"),
        timer(30.0f)
    {
        objRef.setScale(0.75f, 0.75f, 2.0f);
    };

    JOP_GENERIC_COMPONENT_CLONE(Missile);

    void setTarget(jop::WeakReference<jop::Object> o)
    {
        m_target = o;
    }

    void update(const float dt) override
    {
        auto o = getObject();
        if (!m_target.expired())
        {
            glm::vec3 delta = o->getGlobalPosition() - m_target->getGlobalPosition();
            float l = glm::length(delta);
            m_velocity += -glm::normalize(delta) * ((l - 10.f)*(l - 16.f) + 15.f) * dt;
            if (glm::length(m_velocity) > l + 15.f)
            {
                m_velocity = glm::normalize(m_velocity) * (l + 15.f);
            }
            if (l < 1.0f)
            {
                m_target->removeSelf();
                o->removeSelf();
            }
            o->setRotation(glm::quat_cast(glm::lookAt(o->getPosition(), m_velocity, o->getGlobalUp())));
        }
        else
        {
            o->rotate(dt * 2.0f, dt * 2.0f, dt * 2.0f);
            //getObject()->removeSelf();
        }
        o->move(m_velocity * dt);
        timer -= dt;
        if (timer < 0)
        {
            getObject()->removeSelf();
        }
    }
private:
    jop::WeakReference<jop::Object> m_target;
    glm::vec3 m_velocity;
    float timer;
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