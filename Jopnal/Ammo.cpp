#include "Ammo.h"
#include "Enemy.h"
#include "GameController.h"
#include "MapComponent.h"

void Ammo::update(float dt)
{
    //check if collides w/ ground
    glm::vec3 pos = getObject()->getGlobalPosition();
    float y = getObject()->getScene().findChild("terrain")->getComponent<MapComponent>()->terrainY(pos.x, pos.z);

    if (pos.y < y - 100.f)
    {
        getObject()->removeSelf();
    }

    m_timer -= dt;
    if (m_timer < 0)
    {
        getObject()->removeSelf();
    }
}

void Ammo::init()
{
    m_speed = 10.f;
    m_tot_time = 0.0f;
    m_velocity = glm::vec3(0.f, 1.f, 0.f);

    getObject()->createComponent<jop::GenericDrawable>(getObject()->getScene().getRenderer());
    auto drawable = getObject()->getComponent<jop::GenericDrawable>();
    drawable->setModel(jop::Model(jop::Mesh::getDefault(), jop::ResourceManager::getExistingResource<jop::Material>("bulletMaterial")));
}

void Ammo::setVelocity(glm::vec3 velocity)
{
    m_velocity = velocity;
}

void Ammo::setTarget(jop::WeakReference<jop::Object> o)
{
    m_target = o;
}

//// MISSILE ////

void Missile::update(const float dt)
{
    Ammo::update(dt);

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
            auto enemy = m_target->getComponent<Enemy>();
            enemy->setHealth(enemy->getHealth() - (500.f - l));
			o->removeSelf();
		}
	}
	else
	{
        //continue 'till eternity!
		//o->rotate(dt * 2.0f, dt * 2.0f, dt * 2.0f);
		//getObject()->removeSelf();
	}
	o->move(m_velocity * dt * m_speed);
}

void Missile::init()
{
    Ammo::init();

    m_speed = 50.f;
    m_tot_time = 0.0f;
    m_velocity = glm::vec3(0.f, 1.f, 0.f);

    getObject()->setScale(0.75f, 0.75f, 2.0f);
    getObject()->setRotation(3.14159f / 2.f, 0.0f, 0.0f);
}

//// BULLET ////

void Bullet::init()
{
    Ammo::init();

    m_speed = 20.f;
    m_tot_time = 0.0f;
    m_velocity = glm::vec3(0.0f);
    m_timer = 0.5f;

    getObject()->setScale(0.75f, 0.75f, 2.0f);
    getObject()->setRotation(3.14159f / 2.f, 0.0f, 0.0f);
}

void Bullet::setTarget(jop::WeakReference<jop::Object> o)
{
    m_target = o;
    setVelocity(glm::normalize(o->getGlobalPosition() - getObject()->getGlobalPosition()) * 10.f);
}

void Bullet::update(const float dt)
{
    Ammo::update(dt);
    auto o = getObject();

    glm::vec3 pos1 = o->getGlobalPosition();
    o->move(m_velocity * dt * m_speed);
    glm::vec3 pos2 = o->getGlobalPosition();

    if (!m_target.expired())
    {
        glm::vec3 pos3 = m_target->getGlobalPosition();

        //float d = glm::length(glm::cross((pos3 - pos1), (pos3 - pos2))) / glm::length(pos2 - pos1);

        float d;

        glm::vec3 v = pos2 - pos1;
        glm::vec3 w = pos3 - pos1;

        float c1 = glm::dot(w, v);
        float c2 = glm::dot(v, v);
        if (c1 <= 0)
        {
            d = glm::length(pos3 - pos1);
        }
        else if (c2 <= c1)
        {
            d = glm::length(pos3 - pos2);
        }
        else
        {
            float b = c1 / c2;
            glm::vec3 Pb = pos1 + b*v;
            d = glm::length(pos3 - Pb);
        }
        /*glm::vec3 delta = m_target->getGlobalPosition() - o->getGlobalPosition();
        float l = glm::length(delta);*/
        if (d < 1.0f)
        {
            auto enemy = m_target->getComponent<Enemy>();
            enemy->setHealth(enemy->getHealth() - 5.f);
            o->removeSelf();
            return;
        }
    }
}