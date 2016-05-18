#include "Missile.h"
#include "Enemy.h"
#include "GameController.h"

void Missile::setTarget(jop::WeakReference<jop::Object> o)
{
	m_target = o;
}

void Missile::setVelocity(glm::vec3 vel)
{
	m_velocity = vel;
}

void Missile::update(float dt)
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
			o->getScene().findChild("GC")->getComponent<GameController>()->addMoney(m_target->getComponent<Enemy>()->getReward());
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