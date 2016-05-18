#ifndef TOWER_H
#define TOWER_H

//#include <Jopnal/Jopnal.hpp>
#include "Missile.h"

class Tower : public jop::Component
{
public:
    Tower(jop::Object& objRef)
        : jop::Component(objRef, "name")
    {
        objRef.setScale(0.75f, 2.0f, 0.75f);
        m_timer = 0.0f;
        m_rof = 1.0f;
        findTarget();
    };

    Tower(const Tower& misRef, jop::Object& objRef)
        : jop::Component(objRef, "name")
    {
        objRef.setScale(0.75f, 0.75f, 2.0f);
        m_timer = 0.0f;
        m_rof = 1.0f;
        findTarget();
    };

    JOP_GENERIC_COMPONENT_CLONE(Tower);

    void setTarget(jop::WeakReference<jop::Object> o)
    {
        m_target = o;
    }

    void update(const float dt)override
    {
        m_timer -= dt;
        if (m_timer < 0.f)
        {
			act();
        }
    }

    void findTarget()
    {
        //glm::vec3 pos = getObject()->getGlobalPosition();

        //float dist = FLT_MAX;

        //for (auto target : getObject()->getParent()->findChildrenWithTag("target", false))
        //{
        //    glm::vec3 delta = pos - target->getGlobalPosition();
        //    float d = glm::length(delta);
        //    if (d < dist)
        //    {
        //        m_target = target;
        //        JOP_DEBUG_INFO("Target reset.");
        //        dist = d;
        //    }
        //}
        std::vector<jop::WeakReference<jop::Object>> targets = getObject()->getParent()->findChildrenWithTag("target", false);
        if (targets.size() > 0)
            m_target = targets[rand() % (targets.size())];
    }

	virtual void act()
	{
		auto o = getObject();
		if (m_target.get() != nullptr && !m_target.expired())
		{
			auto missile = o->getScene().createChild("Missile");
			missile->setPosition(o->getLocalPosition());

			auto& component = missile->createComponent<Missile>();
			component.setTarget(m_target);
			component.setVelocity(glm::vec3(0.f, 15.f, 0.f));

			missile->createComponent<jop::GenericDrawable>(o->getScene().getRenderer());
			auto drawable = missile->getComponent<jop::GenericDrawable>();
			drawable->setModel(jop::Model(jop::Mesh::getDefault(), jop::ResourceManager::getExistingResource<jop::Material>("bulletMaterial")));

			JOP_DEBUG_INFO("Missile Created");

			m_timer = m_rof;
		}
		else
		{
			JOP_DEBUG_INFO("Finding targets...");

			findTarget();

			m_timer = m_rof * 2.f;
		}
	}

protected:
    jop::WeakReference<jop::Object> m_target;
    float m_timer;
    float m_rof;
};

//TODO: bullet tower, AoE tower, shield, machine gun tower, laser tower...
// production tower?

//TODO: make towers upgradeable
// = no constant valiues, everytinhg can be modified!

#endif