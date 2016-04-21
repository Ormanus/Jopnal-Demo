#ifndef SCENE_GAME_H
#define SCENE_GAME_H

#define PI 3.14159265359

#include <Jopnal/Jopnal.hpp>
#include <random>
#include "Missile.h"

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
    void closed()override
    {
        jop::Engine::exit();
    }
private:

};

class SceneGame : public jop::Scene
{
private:
    std::vector<jop::WeakReference<jop::Object>> m_rings;
    jop::WeakReference<jop::Object> m_camera;
    jop::WeakReference<jop::Object> m_object;
    jop::WeakReference<jop::Object> m_text;
    float m_time;

public:
    SceneGame()
        : jop::Scene("MyScene")
    {
        getRenderer().setMask(3);

        jop::Window* w = jop::Engine::getSubsystem<jop::Window>();
        jop::Engine::getSubsystem<jop::Window>()->setEventHandler<GameEventHandler>(*w);

        w->setClearColor(jop::Color::Black);

        //camera
        m_camera = createChild("cam");
        m_camera->setPosition(0.0f, 0.0f, 0.0f);
        auto cam = m_camera->createChild("view");
        //cam->setPosition(0.0f, 0.0f, 40.0f);
        cam->createComponent<jop::Camera>(getRenderer(), jop::Camera::Projection::Perspective).setFieldOfView(PI / 2.0f);

        //terrain
        generateLevel(&this->getAsObject());

        //text
        m_text = createChild("Text");
        m_text->setPosition(-150.0f, -250.0f, 0.0f);
        m_text->setScale(800.0f, 800.0f, 1.0f);
        jop::Text& text = m_text->createComponent<jop::Text>(getRenderer());
        text.setFont(jop::ResourceManager::getResource<jop::Font, std::string, int>(std::string("novem___.ttf"), 64));
        text.setRenderGroup(1);
        text.setString("Loading...  (not really)");
        text.setColor(jop::Color(1.0f, 0.8f, 0.3f, 1.0f));

        //cube material
        auto& material = jop::ResourceManager::getEmptyResource<jop::Material>("cubeMaterial", jop::Material::Attribute::DefaultLighting);
        material.setMap(jop::Material::Map::Diffuse, jop::ResourceManager::getResource<jop::Texture2D>("textures/Pixel.png", true));

        //ammo material
        auto& material2 = jop::ResourceManager::getEmptyResource<jop::Material>("bulletMaterial", jop::Material::Attribute::DefaultLighting);
        material2.setMap(jop::Material::Map::Diffuse, jop::ResourceManager::getResource<jop::Texture2D>("textures/Bullet.png", true));

        //middle object
        m_object = createChild("globe");
        m_object->createComponent<jop::ModelLoader>().load("SwordMinecraft.obj");
        m_object->setScale(0.1f);

        //skybox
        auto& map = jop::ResourceManager::getResource<jop::Cubemap>("Cubemap/right.png", "Cubemap/left.png", "Cubemap/top.png", "Cubemap/bottom.png", "Cubemap/front.png", "Cubemap/back.png", false);
        m_object->createComponent<jop::SkyBox>(getRenderer()).setMap(map);

        //light
        auto light = createChild("light");
        light->createComponent<jop::LightSource>(getRenderer(), jop::LightSource::Type::Directional).setIntensity(jop::Color(1.0f, 0.5f, 0.0f)).setAttenuation(50);
        light->setRotation(-0.1f, 0.0f, 0.f);
    }

    void preUpdate(const float dt) override
    {
        float time = jop::Engine::getTotalTime() / 2.0f;

        auto eh = jop::Engine::getSubsystem<jop::Window>()->getEventHandler();

        auto cam = findChild("cam");

        float speed = 100.f;

        if (eh->keyDown(jop::Keyboard::Right))
        {
            cam->move(cam->getLocalRight() * speed * dt);
        }
        if (eh->keyDown(jop::Keyboard::Left))
        {
            cam->move(-cam->getLocalRight() * speed * dt);
        }
        if (eh->keyDown(jop::Keyboard::Up))
        {
            cam->move(cam->getLocalFront() * speed * dt);
        }
        if (eh->keyDown(jop::Keyboard::Down))
        {
            cam->move(-cam->getLocalFront() * speed * dt);
        }

        m_time += dt;
    }
private:
    void createRing(jop::Object* o, float radius)
    {
        const int numCubes = 32;
        const float step = PI / (static_cast<float>(numCubes) / 2.0f);
        const float d = step * radius;
        for (int i = 0; i < 32; i++)
        {
            auto c = o->createChild("cube");
            c->setPosition(cos(i * step) * radius, sin(i * step) * radius, 0.0f);
            c->setRotation(0.0f, 0.0f, i * step);
            c->setScale(1.0f, d, 1.0f);

            c->createComponent<jop::GenericDrawable>(getRenderer());
            auto drawable = c->getComponent<jop::GenericDrawable>();
            drawable->setModel(jop::Model(jop::Mesh::getDefault(), jop::ResourceManager::getExistingResource<jop::Material>("cubeMaterial")));

            c->addTag("target");
        }
    }
    void generateLevel(jop::Object* o)
    {
        auto terrain = o->createChild("terrain");
        auto& material1 = jop::ResourceManager::getEmptyResource<jop::Material>("terrainMaterial", jop::Material::Attribute::DefaultLighting);
        material1.setMap(jop::Material::Map::Diffuse, jop::ResourceManager::getResource<jop::Texture2D>("textures/Bullet.png", true));

        std::vector<jop::Vertex> vertices;

        //jop::Randomizer r;

        const int w = 64;
        const int h = 64;

        for (int i = 0; i < w; i++)
        {
            for (int j = 0; j < h; j++)
            {
                jop::Vertex v;
                float dx = i - w / 2.0f;
                float dy = j - h / 2.0f;

                v.position.y = sin(static_cast<float>(i) / 5.f)*sin(static_cast<float>(j) / 5.f)*8.0f;

                v.position.x = i * 2.0f;
                v.position.z = j * 2.0f;
                v.texCoords.x = 0.0f;
                v.texCoords.y = 0.0f;
                vertices.push_back(v);
                v.position.z = (j + 1.0f) * 2.0f;
                v.texCoords.y = 1.0f;
                vertices.push_back(v);
                v.position.x = (i + 1.0f) * 2.0f;
                v.texCoords.x = 1.0f;
                vertices.push_back(v);
                vertices.push_back(v);
                v.position.z = j * 2.0f;
                v.texCoords.y = 0.0f;
                vertices.push_back(v);
                v.position.x = i * 2.0f;
                v.texCoords.x = 0.0f;
                vertices.push_back(v);
            }
        }

        terrain->setPosition(-w*2.0f, -2.0f, -h*2.0f).setScale(2.0f, 2.0f, 2.0f);;

        auto& mesh = jop::ResourceManager::getNamedResource<jop::Mesh>("mesh", vertices, std::vector<unsigned int>());
        auto& drawable1 = terrain->createComponent<jop::GenericDrawable>(getRenderer());
        drawable1.setModel(jop::Model(mesh, material1));
    }
};

#endif