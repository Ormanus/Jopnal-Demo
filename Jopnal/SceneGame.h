#ifndef SCENE_GAME_H
#define SCENE_GAME_H

#define PI 3.14159265359

#include <Jopnal/Jopnal.hpp>
#include <random>
#include "Tower.h"

class GameEventHandler
    : public jop::WindowEventHandler
{
public:
    GameEventHandler(jop::Window& w, jop::Window& w2)
        :jop::WindowEventHandler(w)
    {
        w.setMouseMode(jop::Mouse::Mode::Frozen);

    }
    ~GameEventHandler(){}

    void keyPressed(const int key, const int, const int)override
    {
        auto& cam = *jop::Engine::getCurrentScene().findChild("cam");

        if (key == jop::Keyboard::Escape)
        {
            closed();
        }
    }

    void mouseMoved(const float x, const float y) override
    {
        if (!jop::Engine::exiting() && jop::Engine::hasCurrentScene())
        {
            auto& cam = *jop::Engine::getCurrentScene().findChild("cam");

            static float mx = 0.f;
            static float my = 0.f;
            mx += x;
            my = glm::clamp(my + y, -85.f, 85.f);

            cam.setRotation(glm::radians(-my), glm::radians(-mx), 0.f);
        }
    }

    void mouseButtonReleased(const int button, const int)override
    {
        if (button == jop::Mouse::Button::Left)
        {
            if (!jop::Engine::exiting() && jop::Engine::hasCurrentScene())
            {
                if (jop::Engine::getCurrentScene().findChildrenWithTag("sword", false).empty())
                {
                    JOP_DEBUG_INFO("404 Sword not found.");
                    return;
                }

                auto& sword = *jop::Engine::getCurrentScene().findChildrenWithTag("sword", false)[0];
                glm::vec3 pos = sword.getLocalPosition();

                //create tower
                auto tower = jop::Engine::getCurrentScene().createChild("tower");
                tower->setPosition(pos + glm::vec3(0.f, 1.f, 0.f));
                tower->createComponent<Tower>();
                auto& drawable = tower->createComponent<jop::GenericDrawable>(jop::Engine::getCurrentScene().getRenderer());
                drawable.setModel(jop::Model(jop::Mesh::getDefault(), jop::ResourceManager::getExistingResource<jop::Material>("cubeMaterial")));

                JOP_DEBUG_INFO("Tower Created.");
            }
        }
    }

    void closed()override
    {
        jop::Engine::exit();
    }
private:

};

class SceneGame : public jop::Scene
{
private:
    jop::WeakReference<jop::Object> m_world;
    jop::WeakReference<jop::Object> m_object;
    jop::WeakReference<jop::Object> m_text;
    float m_time;
    float m_enemyTimer;
public:
    SceneGame()
        : jop::Scene("MyScene")
    {
        //getRenderer().setMask(3);

        jop::Window* w = jop::Engine::getSubsystem<jop::Window>();
        jop::Engine::getSubsystem<jop::Window>()->setEventHandler<GameEventHandler>(*w);

        w->setClearColor(jop::Color::Black);

        //getWorld().setDebugMode(true);

        //camera
        auto camObj = createChild("cam");
        camObj->setPosition(0.0f, 0.0f, 0.0f);
        auto cam = camObj->createChild("view");
        //cam->setPosition(0.0f, 0.0f, 40.0f);
        cam->createComponent<jop::Camera>(getRenderer(), jop::Camera::Projection::Perspective).setFieldOfView(PI / 2.0f);

        //terrain
        generateLevel(&this->getAsObject());

        //cube material
        auto& material = jop::ResourceManager::getEmptyResource<jop::Material>("cubeMaterial", jop::Material::Attribute::DefaultLighting);
        material.setMap(jop::Material::Map::Diffuse, jop::ResourceManager::getResource<jop::Texture2D>("textures/Pixel.png", true));

        //ammo material
        auto& material2 = jop::ResourceManager::getEmptyResource<jop::Material>("bulletMaterial", jop::Material::Attribute::DefaultLighting);
        material2.setMap(jop::Material::Map::Diffuse, jop::ResourceManager::getResource<jop::Texture2D>("textures/Bullet.png", true));

        //sword
        m_object = createChild("sword");
        m_object->addTag("sword");
        m_object->createComponent<jop::ModelLoader>().load("SwordMinecraft.obj");
        m_object->setScale(0.1f);
        m_object->setRotation(0.0f, PI / 4.f, PI / 2.0f);

        //skybox
        auto& map = jop::ResourceManager::getResource<jop::Cubemap>("Cubemap/right.png", "Cubemap/left.png", "Cubemap/top.png", "Cubemap/bottom.png", "Cubemap/front.png", "Cubemap/back.png", false);
        m_object->createComponent<jop::SkyBox>(getRenderer()).setMap(map);

        //light
        auto light = createChild("light");
        light->createComponent<jop::LightSource>(getRenderer(), jop::LightSource::Type::Directional).setIntensity(jop::Color(1.0f, 0.9f, 0.0f)).setAttenuation(50);
        light->setRotation(-0.1f, 0.0f, 0.f);
    }

    void preUpdate(const float dt) override
    {
        float time = jop::Engine::getTotalTime() / 2.0f;

        auto eh = jop::Engine::getSubsystem<jop::Window>()->getEventHandler();

        auto cam = findChild("cam");

        float speed = 10.f;

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
            cam->move(cam->getLocalFront() * speed * dt);
        }
        if (eh->keyDown(jop::Keyboard::S))
        {
            cam->move(-cam->getLocalFront() * speed * dt);
        }

        m_time += dt;

        auto collider = findChild("terrain")->getComponent<jop::RigidBody>();

        if (m_time > 0.01f)
        {
            m_time -= 0.01f;
            jop::RayInfo ray = getWorld().checkRayClosest(cam->getGlobalPosition(), cam->getGlobalFront() * 1000.f);
            if (ray.point != glm::vec3(0))
            {
                m_object->setPosition(ray.point);
            }
        }

        m_enemyTimer += dt;
        if (m_enemyTimer > 1.0f)
        {
            m_enemyTimer -= 1.0f;

            float t = jop::Engine::getTotalTime();

            auto o = createChild("TargetObject");
            o->setPosition(cos(t) * 10.f + 64.f, sin(t * 5.0f) * 5.f + 32.f, sin(t) * 10.f + 64.f);
            o->createComponent<jop::GenericDrawable>(getRenderer());
            o->addTag("target");
            auto drawable = o->getComponent<jop::GenericDrawable>();
            drawable->setModel(jop::Model(jop::Mesh::getDefault(), jop::ResourceManager::getExistingResource<jop::Material>("bulletMaterial")));
        }
    }
private:
    void generateLevel(jop::Object* o)
    {
        const float scale = 10.0f;

        auto terrain = o->createChild("terrain");
        auto& material1 = jop::ResourceManager::getEmptyResource<jop::Material>("terrainMaterial", jop::Material::Attribute::DefaultLighting);
        material1.setMap(jop::Material::Map::Diffuse, jop::ResourceManager::getResource<jop::Texture2D>("textures/Bullet.png", true));

        std::vector<jop::Vertex> vertices;

        const int w = 32;
        const int h = 32;

        std::vector<glm::vec3> points;

        for (int i = 0; i < w; i++)
        {
            for (int j = 0; j < h; j++)
            {
                //meshify
                jop::Vertex v;
                float dx = i - w / 2.0f;
                float dy = j - h / 2.0f;

                v.position.x = i * scale;
                v.position.z = j * scale;
                v.position.y = terrainY(v.position.x, v.position.z);
                v.texCoords.x = 0.0f;
                v.texCoords.y = 0.0f;
                vertices.push_back(v);
                points.push_back(v.position);

                v.position.z = (j + 1.0f) * scale;
                v.position.y = terrainY(v.position.x, v.position.z);
                v.texCoords.y = 1.0f;
                vertices.push_back(v);
                points.push_back(v.position);

                v.position.x = (i + 1.0f) * scale;
                v.position.y = terrainY(v.position.x, v.position.z);
                v.texCoords.x = 1.0f;
                vertices.push_back(v);
                vertices.push_back(v);
                points.push_back(v.position);
                points.push_back(v.position);

                v.position.z = j * scale;
                v.position.y = terrainY(v.position.x, v.position.z);
                v.texCoords.y = 0.0f;
                vertices.push_back(v);
                points.push_back(v.position);

                v.position.x = i * scale;
                v.position.y = terrainY(v.position.x, v.position.z);
                v.texCoords.x = 0.0f;
                vertices.push_back(v);
                points.push_back(v.position);
            }
        }
        terrain->createComponent<jop::RigidBody>(getWorld(), jop::RigidBody::ConstructInfo(jop::ResourceManager::getNamedResource<jop::TerrainShape>("mesh", points)));
        auto& mesh = jop::ResourceManager::getNamedResource<jop::Mesh>("mesh", vertices, std::vector<unsigned int>());
        auto& drawable1 = terrain->createComponent<jop::GenericDrawable>(getRenderer());
        drawable1.setModel(jop::Model(mesh, material1));
    }

    float terrainY(float x, float z)
    {
        return sin(x * 5.f)*sin(z * 5.f)*10.0f;
    }
};

#endif