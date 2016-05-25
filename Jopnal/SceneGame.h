#ifndef SCENE_GAME_H
#define SCENE_GAME_H

#define PI 3.14159265359

//#include <Jopnal/Jopnal.hpp>
#include <random>
//#include "MapComponent.h"
#include "Tower.h"
//#include "HUDComponent.h"
#include "Enemy.h"
#include "GameController.h"
#include "SceneStart.h"

//----- Event handler -----
class GameEventHandler
    : public jop::WindowEventHandler
{
public:
    GameEventHandler(jop::Window& w, jop::Window& w2)
        :jop::WindowEventHandler(w)
    {
        //w.setMouseMode(jop::Mouse::Mode::Frozen);
		m_mx = 0.f;
		m_my = 0.f;
    }
    ~GameEventHandler(){}

    void keyPressed(const int key, const int, const int)override
    {
        auto& cam = *jop::Engine::getCurrentScene().findChild("cam");

        if (key == jop::Keyboard::Escape)
        {
            closed();
        }
		else if (key == jop::Keyboard::LShift)
		{
			jop::Engine::getSubsystem<jop::Window>()->setMouseMode(jop::Mouse::Mode::Frozen);
		}
    }

    void mouseMoved(const float x, const float y) override
    {
        if (!jop::Engine::exiting() && jop::Engine::hasCurrentScene())
        {
            if (keyDown(jop::Keyboard::LShift))
            {
                auto& cam = *jop::Engine::getCurrentScene().findChild("cam");

                m_mx += x;
                m_my = glm::clamp(m_my + y, -85.f, 85.f);

                cam.setRotation(glm::radians(-m_my), glm::radians(-m_mx), 0.f);
            }

            else
            {
                jop::Engine::getSubsystem<jop::Window>()->setMouseMode(jop::Mouse::Mode::Visible);
            }
            //send mouse movement to hud elements

            for (auto o : jop::Engine::getCurrentScene().findChildrenWithTag("HUD", true))
            {
                auto c = o->getComponent<Button>();
                if (c != nullptr)
                {
                    c->mouseMove(x, y);
                }
            }

            //debug mouse position
            //jop::Engine::getCurrentScene().findChild("DEBUG")->getComponent<jop::Text>()->setString("MPos: " + std::to_string(x) + ", " + std::to_string(y));
        }
    }

    void mouseButtonReleased(const int button, const int)override
    {
        if (button == jop::Mouse::Button::Left)
        {
            if (!jop::Engine::exiting() && jop::Engine::hasCurrentScene())
            {
                jop::Engine::getCurrentScene().findChild("GC")->getComponent<GameController>()->mouseLeft();
            }
        }
    }

    void closed()override
    {
        jop::Engine::exit();
    }
private:
	float m_mx;
	float m_my;
	bool m_firstShift;
};


//----- Scene -----
class SceneGame : public jop::Scene
{
private:
    jop::WeakReference<jop::Object> m_world;
    jop::WeakReference<jop::Object> m_object;
    jop::WeakReference<jop::Object> m_text;
    float m_time;
    float m_enemyTimer;
	float m_enemySize;
    //glm::vec3* m_path;
    //const int m_numWaypoints = 16;
public:
    SceneGame()
        : jop::Scene("MyScene")
    {
        getRenderer().setMask(3);

        jop::Window* w = jop::Engine::getSubsystem<jop::Window>();
        jop::Engine::getSubsystem<jop::Window>()->setEventHandler<GameEventHandler>(*w);

        w->setClearColor(jop::Color::Black);

        //getWorld().setDebugMode(true);

        //camera
        auto cam = createChild("cam");
        cam->setPosition(0.0f, 30.0f, 0.0f);
        cam->createComponent<jop::Camera>(getRenderer(), jop::Camera::Projection::Perspective).setFieldOfView(PI / 2.0f);

        //terrain
        const float levelScale = 10.f;

		auto map = createChild("map");
		auto& mapComp = map->createComponent<MapComponent>();
		mapComp.generateLevel(&this->getAsObject(), levelScale);
		mapComp.generatePath(levelScale);

        using A = jop::Material::Attribute;

        //cube material
        auto& material = jop::ResourceManager::getEmptyResource<jop::Material>("cubeMaterial", A::DefaultLighting);
		material.setMap(jop::Material::Map::Diffuse, jop::ResourceManager::getResource<jop::Texture2D>("textures/Pixel.png", true, false));

        //ammo material
        auto& material2 = jop::ResourceManager::getEmptyResource<jop::Material>("bulletMaterial", A::DiffuseMap);
		material2.setMap(jop::Material::Map::Diffuse, jop::ResourceManager::getResource<jop::Texture2D>("textures/Bullet.png", true, false));
        
        //force field material
        auto& ffMat = jop::ResourceManager::getEmptyResource<jop::Material>("ffMaterial", A::DefaultLighting | A::DiffuseAlpha/* | A::Alpha*/);
		ffMat.setMap(jop::Material::Map::Diffuse, jop::ResourceManager::getResource<jop::Texture2D>("textures/FFTexture.png", true, false));

        //TODO: placement materials, red and green!

        //sword
        m_object = createChild("sword");
        m_object->addTag("sword");
        m_object->createComponent<jop::ModelLoader>().load("SwordMinecraft.obj");
        m_object->setScale(0.1f);
        m_object->setRotation(0.0f, PI / 4.f, PI / 2.0f);

        //skybox
		auto& cubeMap = jop::ResourceManager::getResource<jop::Cubemap>("Cubemap/right.png", "Cubemap/left.png", "Cubemap/top.png", "Cubemap/bottom.png", "Cubemap/front.png", "Cubemap/back.png", true, false);
		m_object->createComponent<jop::SkyBox>(getRenderer()).setMap(cubeMap);

        //light
        auto light = createChild("light");
        light->createComponent<jop::LightSource>(getRenderer(), jop::LightSource::Type::Directional).setIntensity(jop::Color(1.0f, 0.9f, 0.3f)).setAttenuation(50);
        light->setRotation(-0.3f, 0.0f, 0.f);

        //Game Controller
        createChild("GC")->createComponent<GameController>();

        //HUD
        //cam
        auto orthoCam = createChild("orthoCam");
        orthoCam->setPosition(0.0f, 0.0f, 0.0f);
        auto cam2 = orthoCam->createChild("view");
        cam2->createComponent<jop::Camera>(getRenderer(), jop::Camera::Projection::Orthographic).setRenderMask(1 << 1);
        cam2->setActive(true);

        //buttons
        createButton(glm::vec2(480.0f, 000.0f), glm::vec2(-1.0f), "textures/button_bullet.png", "textures/button_bullet_down.png", "textures/button_bullet_hover.png")->setMessage(0, "1");
        createButton(glm::vec2(480.0f, -120.0f), glm::vec2(-1.0f), "textures/button_missile.png", "textures/button_missile_down.png", "textures/button_missile_hover.png")->setMessage(0, "2");;
        createButton(glm::vec2(480.0f, -240.0f), glm::vec2(-1.0f), "textures/button_shield.png", "textures/button_shield_down.png", "textures/button_shield_hover.png")->setMessage(0, "3");;

        //degug display
        auto debugObject = createChild("DEBUG");
        debugObject->setScale(800.0f, 800.0f, 1.0f).setPosition(0.0f, 0.0f, 1.f);
        jop::Text& text = debugObject->createComponent<jop::Text>(getRenderer());
        text.setFont(jop::ResourceManager::getResource<jop::Font, std::string, int>(std::string("fonts/novem___.ttf"), 64));
        text.setRenderGroup(1);
        text.setString("Mouse Position: ");
        text.setColor(jop::Color(0.0f, 0.9f, 0.0f, 1.0f));
        
        //jop::Engine::sendMessage("[Co] setAction 2");

		m_enemySize = 1.0f;
    }

    void preUpdate(const float dt) override
    {
        float time = jop::Engine::getTotalTime() / 2.0f;

        auto eh = jop::Engine::getSubsystem<jop::Window>()->getEventHandler();

		auto& scene = getAsObject();

        auto cam = findChild("cam");

        float speed = 20.f;

        if (eh->keyDown(jop::Keyboard::D))
        {
            cam->move(cam->getLocalRight() * speed * dt);
        }
        if (eh->keyDown(jop::Keyboard::A))
        {
            cam->move(-cam->getLocalRight() * speed * dt);
        }
        if (eh->keyDown(jop::Keyboard::W))
        {
			cam->move(glm::cross(scene.getGlobalUp(), cam->getLocalRight()) * speed * dt);
        }
        if (eh->keyDown(jop::Keyboard::S))
        {
			cam->move(-glm::cross(scene.getGlobalUp(), cam->getLocalRight()) * speed * dt);
        }

        m_time += dt;

        auto collider = findChild("terrain")->getComponent<jop::RigidBody>();

        if (m_time > 0.01f && collider != nullptr)
        {
            m_time -= 0.01f;
            jop::RayInfo ray;
            if (eh->keyDown(jop::Keyboard::LShift))
            {
                ray = getWorld().checkRayClosest(cam->getGlobalPosition(), cam->getGlobalFront() * 1000.f);
            }
            else
            {
                ray = getWorld().checkRayClosest(cam->getGlobalPosition(), cam->getComponent<jop::Camera>()->getPickRay(eh->getCursorPosition(), jop::Engine::getMainRenderTarget()) * 1000.f);   
            }
            
            if (ray.point != glm::vec3(0))
            {
                m_object->setPosition(ray.point);
            }
        }

        m_enemyTimer += dt;
        if (m_enemyTimer > m_enemySize)
        {
			m_enemyTimer -= m_enemySize;

            float t = jop::Engine::getTotalTime();

			auto map = findChild("map");

            createEnemy(10.f, 10.f, 0)->getComponent<Enemy>()->setPath(map->getComponent<MapComponent>()->getPath());
        }
    }

	void postUpdate(const float dt)override;

    void gameOver()
    {
        m_gameover = true;
    }

private:
    
    jop::WeakReference<jop::Object> createEnemy(float x, float y, int type)
    {
        //TODO: switch(type) & different enemy types
        auto o = createChild("enemy");
        o->createComponent<Enemy>().setHealth(m_enemySize * 100.f);
		m_enemySize += 0.1f;
        return o;
    }

    Button* createButton(glm::vec2 position, glm::vec2 size, std::string path1, std::string path2, std::string path3)
    {
		auto gc = findChild("GC")->getComponent<GameController>();

        auto o = findChild("orthoCam")->createChild("button");
        o->createComponent<Button>(path1, path2, path3, gc);
        o->setPosition(glm::vec3(position.x, position.y, 1.0f)).setScale(glm::vec3(size.x, size.y, 1.0f));
        return o->getComponent<Button>();
    }

    bool m_gameover;
};

#endif