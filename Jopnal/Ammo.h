#ifndef MISSILE_H
#define MISSILE_H

#include "MapComponent.h" //<Jopnal/Jopnal.hpp>
//#include "GameController.h"

class GameController;

class Ammo : public jop::Component
{
public:

    Ammo(jop::Object& objRef)
        : jop::Component(objRef, "ammo"),
        m_timer(10.0f)
    {
    };

    Ammo(const Ammo& misRef, jop::Object& objRef)
        : jop::Component(objRef, "ammo"),
        m_timer(10.0f)
    {
    };

    JOP_GENERIC_COMPONENT_CLONE(Ammo);

    virtual void init();

    virtual void setTarget(jop::WeakReference<jop::Object> o);

    virtual void setVelocity(glm::vec3 vel);

    virtual void update(const float dt) override;
protected:
    jop::WeakReference<jop::Object> m_target;
    glm::vec3 m_velocity;
    float m_timer;
    float m_speed;
    float m_tot_time;
};

class Missile : public Ammo
{
public:

    Missile(jop::Object& objRef)
        : Ammo(objRef)
    {
        init();
    };

    Missile(const Missile& misRef, jop::Object& objRef)
        : Ammo(objRef)
    {
        init();
    };

    JOP_GENERIC_COMPONENT_CLONE(Missile);

    //void init()override;
	void update(const float dt) override;

    void init()override;
};

////  BULLET CLASS  ////

//TODO: redo! :P

#endif