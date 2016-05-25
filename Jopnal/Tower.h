#ifndef TOWER_H
#define TOWER_H

//#include <Jopnal/Jopnal.hpp>
#include "Ammo.h"
#include "Enemy.h"
#include "ShieldDrawable.h"

class Tower : public jop::Component
{
public:
    Tower(jop::Object& objRef)
        : jop::Component(objRef, 0)
    {
        init();
    };

    //Tower(const Tower& misRef, jop::Object& objRef)
    //    : jop::Component(objRef, "name")
    //{
    //    init();
    //};

    //JOP_GENERIC_COMPONENT_CLONE(Tower);
    JOP_DISALLOW_COPY(Tower);

    virtual void init()
    {
        auto o = getObject();

        //rigid body
        auto& box = o->createComponent<jop::RigidBody>(o->getScene().getWorld(), jop::RigidBody::ConstructInfo(jop::ResourceManager::getNamedResource<jop::BoxShape>("shape", glm::vec3(0.75f, 2.f, 0.75f))));
        
        m_timer = 0.0f;
        m_rof = 1.0f;
        m_range = 10000.f;
        m_targetingType = 0;
        findTarget();
    }

    void setTarget(jop::WeakReference<jop::Object> o)
    {
        m_target = o;
    }

    void update(const float dt)override
    {
        m_timer -= dt;
        if (m_timer < 0.f)
        {
            m_timer = m_rof;
			act();
        }
    }

    void findTarget()
    {
        switch (m_targetingType)
        {
        case 0: //closest
        {
            glm::vec3 pos = getObject()->getGlobalPosition();

            float dist = FLT_MAX;

            for (auto target : getObject()->getParent()->findChildrenWithTag("target", false))
            {
                glm::vec3 delta = pos - target->getGlobalPosition();
                float d = glm::length(delta);
                if (d < dist && d < m_range)
                {
                    m_target = target;
                    dist = d;
                }
            }
            break;
        }
        case 1: //strongest
        {
            float hp = 0;

            for (auto target : getObject()->getParent()->findChildrenWithTag("target", false))
            {
                glm::vec3 delta = getObject()->getGlobalPosition() - target->getGlobalPosition();
                float d = glm::length(delta);
                if (target->getComponent<Enemy>()->getHealth() > hp && d < m_range)
                {
                    m_target = target;
                    hp = target->getComponent<Enemy>()->getHealth();
                }
            }
            break;
        }
        case 2: //first
        {
            float dist = 0;

            for (auto target : getObject()->getParent()->findChildrenWithTag("target", false))
            {
                glm::vec3 delta = getObject()->getGlobalPosition() - target->getGlobalPosition();
                float l = glm::length(delta);
                float d = target->getComponent<Enemy>()->getDistanceFromHome();
                if (d > dist && l < m_range)
                {
                    m_target = target;
                    dist = d;
                }
            }
            break;
        }
        case 3: //last
        {
            float dist = FLT_MAX;

            for (auto target : getObject()->getParent()->findChildrenWithTag("target", false))
            {
                float d = target->getComponent<Enemy>()->getDistanceFromHome();
                glm::vec3 delta = getObject()->getGlobalPosition() - target->getGlobalPosition();
                float l = glm::length(delta);
                if (d < dist && l < m_range)
                {
                    m_target = target;
                    dist = d;
                }
            }
            break;
        }
        case 4: //random
        {
            std::vector<jop::WeakReference<jop::Object>> targets = getObject()->getParent()->findChildrenWithTag("target", false);
            if (targets.size() > 0)
                m_target = targets[rand() % (targets.size())];
            JOP_DEBUG_INFO("Target reset to random.");
            break;
        }  
        default:
            break;
        }
        
        
    }

    virtual void act() = 0;

protected:
    jop::WeakReference<jop::Object> m_target;
    float m_timer;
    float m_rof;
    float m_range;
    int m_targetingType;
};

//TODO: bullet tower, AoE tower, shield, machine gun tower, laser tower...
// production tower?

//// MISSILE TOWER ////

class MissileTower : public Tower
{
public:
    MissileTower(jop::Object& objRef)
        : Tower(objRef)
    {
        init();
    };

    MissileTower(const MissileTower& misRef, jop::Object& objRef)
        : Tower(objRef)
    {
        init();
    };

    JOP_GENERIC_COMPONENT_CLONE(MissileTower);

    void init()override
    {
        Tower::init();
        m_timer = 0.0f;
        m_rof = 2.0f;
        m_targetingType = 2;
        m_range = 1000.f;

        auto o = getObject();
        auto& drawable = o->createComponent<jop::GenericDrawable>(jop::Engine::getCurrentScene().getRenderer());
        drawable.setModel(jop::Model(jop::Mesh::getDefault(), jop::ResourceManager::getExistingResource<jop::Material>("cubeMaterial")));
        o->setScale(0.75f, 2.0f, 0.75f);
    }

    void act()override
    {
        auto o = getObject();
        if (m_target.get() != nullptr && !m_target.expired())
        {
            auto missile = o->getScene().createChild("Missile");
            missile->setPosition(o->getLocalPosition());

            auto& component = missile->createComponent<Missile>();
            component.setTarget(m_target);
            component.setVelocity(glm::vec3(0.f, 15.f, 0.f));
            m_timer = m_rof;
        }
        else
        {
            findTarget();
            m_timer = m_rof * 2.f;
        }
    }
};

//// BULLET TOWER ////

class BulletTower : public Tower
{
public:
    BulletTower(jop::Object& objRef)
        : Tower(objRef)
    {
        init();
    };

    BulletTower(const BulletTower& misRef, jop::Object& objRef)
        : Tower(objRef)
    {
        init();
    };

    JOP_GENERIC_COMPONENT_CLONE(BulletTower);

    void init()override
    {
        Tower::init();
        m_timer = 0.0f;
        m_rof = 0.25f;
        m_targetingType = 0;
        m_range = 50.f;

        auto o = getObject();
        auto& drawable = o->createComponent<jop::GenericDrawable>(jop::Engine::getCurrentScene().getRenderer());
        drawable.setModel(jop::Model(jop::Mesh::getDefault(), jop::ResourceManager::getExistingResource<jop::Material>("cubeMaterial")));
        o->setScale(0.75f, 2.0f, 0.75f);
    }

    void act()override
    {
        auto o = getObject();
        if (m_target.get() != nullptr && !m_target.expired())
        {
            glm::vec3 delta = getObject()->getGlobalPosition() - m_target->getGlobalPosition();
            float d = glm::length(delta);
            if (d < m_range)
            {
                auto missile = o->getScene().createChild("Missile");
                missile->setPosition(o->getLocalPosition());

                auto& component = missile->createComponent<Bullet>();
                component.setTarget(m_target);
                m_timer = m_rof;
                findTarget();

                return;
            }
        }
        findTarget();
        m_timer = m_rof * 2.f;
    }
};

//// FORCE FIELD TOWER ////

class FFTower : public Tower
{
public:
    FFTower(jop::Object& objRef)
        : Tower(objRef)
    {
        init();
    };

    FFTower(const FFTower& misRef, jop::Object& objRef)
        : Tower(objRef)
    {
        init();
    };

    JOP_GENERIC_COMPONENT_CLONE(FFTower);

    void init()override
    {
        Tower::init();

        auto o2 = getObject()->createChild("center");
        auto& drawable2 = o2->createComponent<jop::GenericDrawable>(jop::Engine::getCurrentScene().getRenderer());
        drawable2.setModel(jop::Model(jop::Mesh::getDefault(), jop::ResourceManager::getExistingResource<jop::Material>("cubeMaterial")));
        o2->setScale(0.75f, 2.0f, 0.75f);

        //add ff model
        auto o = getObject();
        auto& drawable = o->createComponent<ShieldDrawable>(o->getScene().getRenderer());
        drawable.setModel(jop::Model(jop::ResourceManager::getNamedResource<jop::SphereMesh>("ff", 16.f, 32, 32), jop::ResourceManager::getExistingResource<jop::Material>("ffMaterial")));

        m_timer = 0.0f;
        m_rof = 0.1f;
        m_targetingType = 0;
        m_range = 16.f;
    }

    void act()override
    {
        auto o = getObject();
        glm::vec3 pos = getObject()->getGlobalPosition();

        for (auto target : getObject()->getParent()->findChildrenWithTag("target", false))
        {
            glm::vec3 delta = o->getGlobalPosition() - target->getGlobalPosition();
            if (glm::length(delta) < m_range)
            {
                auto enemy = target->getComponent<Enemy>();
                enemy->setHealth(enemy->getHealth() - 2.5f);
            }
        }

        m_timer = m_rof;
    }
};

#endif