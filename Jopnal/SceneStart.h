#ifndef SCENE_START_H
#define SCENE_START_H

#define PI 3.14159265359

#include <Jopnal/Jopnal.hpp>
#include "Missile.h"
#include "HUDComponent.h"

class EventHandler
    : public jop::WindowEventHandler
{
public:
    EventHandler(jop::Window& w, jop::Window& w2)
        :jop::WindowEventHandler(w)
    {
        w.setMouseMode(jop::Mouse::Mode::Frozen);
        
    }
    ~EventHandler(){}

    void keyPressed(const int key, const int, const int)override
    {
        if (key == jop::Keyboard::Escape)
        {
            closed();
        }
        if (key == jop::Keyboard::Right)
        {
            jop::broadcast("[Ob=cam] rotate 0.f -0.1f 0.f");
        }
        if (key == jop::Keyboard::Left)
        {
            jop::broadcast("[Ob=cam] rotate 0.f 0.1f 0.f");
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

class SceneStart : public jop::Scene
{
private:
    std::vector<jop::WeakReference<jop::Object>> m_rings;
    jop::WeakReference<jop::Object> m_camera;
    jop::WeakReference<jop::Object> m_object;
    jop::WeakReference<jop::Object> m_text;
    float m_time;

public:
    SceneStart()
        : jop::Scene("MyScene")
    {
        getRenderer().setMask(3);

        jop::Window* w = jop::Engine::getSubsystem<jop::Window>();
        jop::Engine::getSubsystem<jop::Window>()->setEventHandler<EventHandler>(*w);

        w->setClearColor(jop::Color::Black);

        //camera
        m_camera = createChild("cam");
        m_camera->setPosition(0.0f, 0.0f, 0.0f);
        auto cam = m_camera->createChild("view");
        cam->setPosition(0.0f, 0.0f, 40.0f);
        cam->createComponent<jop::Camera>(getRenderer(), jop::Camera::Projection::Perspective).setFieldOfView(PI / 2.0f);

        //HUD
        //cam
        auto orthoCam = createChild("orthoCam");
        orthoCam->setPosition(0.0f, 0.0f, 0.0f);
        auto cam2 = orthoCam->createChild("view");
        cam2->createComponent<jop::Camera>(getRenderer(), jop::Camera::Projection::Orthographic).setRenderMask(1 << 1);
        cam2->setActive(true);

        //crosshair
        //auto crosshair = cam2->createChild("crosshair");
        //crosshair->createComponent<jop::GenericDrawable>(getRenderer());
        //auto drawable1 = crosshair->getComponent<jop::GenericDrawable>();
        //drawable1->setRenderGroup(1);
        //cam2->findChild("crosshair")->setPosition(0.0f, 0.0f, 0.0f).setScale(1.0f, 1.0f, 1.0f);
        //auto& material1 = jop::ResourceManager::getEmptyResource<jop::Material>("cubeMaterial2", jop::Material::Attribute::Default);
        //material1.setMap(jop::Material::Map::Diffuse, jop::ResourceManager::getResource<jop::Texture2D>("crosshair.png"));
        //drawable1->setModel(jop::Model(jop::ResourceManager::getNamedResource<jop::RectangleMesh>("rectangle", 100.0f), material1));

        //text
        m_text = createChild("Text");
        m_text->setPosition(-150.0f, -250.0f, 0.0f);
        m_text->setScale(800.0f, 800.0f, 1.0f);
        jop::Text& text = m_text->createComponent<jop::Text>(getRenderer());
        text.setFont(jop::ResourceManager::getResource<jop::Font, std::string, int>(std::string("fonts/novem___.ttf"), 64));
        text.setRenderGroup(1);
        text.setString("Loading...  (not really)");
        text.setColor(jop::Color(1.0f, 0.8f, 0.3f, 1.0f));

        //cube material
        auto& material = jop::ResourceManager::getEmptyResource<jop::Material>("cubeMaterial", jop::Material::Attribute::DefaultLighting);
        material.setMap(jop::Material::Map::Diffuse, jop::ResourceManager::getResource<jop::Texture2D>("textures/Pixel.png", true));

        //bullet material
        auto& material2 = jop::ResourceManager::getEmptyResource<jop::Material>("bulletMaterial", jop::Material::Attribute::DefaultLighting);
        material2.setMap(jop::Material::Map::Diffuse, jop::ResourceManager::getResource<jop::Texture2D>("textures/Bullet.png", true));

        //sword
        m_object = createChild("sword");
        m_object->createComponent<jop::ModelLoader>().load("SwordMinecraft.obj");
        m_object->setScale(0.1f);

        //skybox
        auto& map = jop::ResourceManager::getResource<jop::Cubemap>("Cubemap/right.png", "Cubemap/left.png", "Cubemap/top.png", "Cubemap/bottom.png", "Cubemap/front.png", "Cubemap/back.png", false);
        m_object->createComponent<jop::SkyBox>(getRenderer()).setMap(map);

        //create rings
        m_rings.push_back(createChild("ring"));
        createRing(m_rings[0].get(), 8.0f);

        for (int i = 1; i < 6; i++)
        {
            m_rings.push_back(m_rings[i - 1]->createChild("ring"));
            createRing(m_rings[i].get(), 8.0f + i*3.0f);
        }

        //light
        auto light = createChild("light");
        light->createComponent<jop::LightSource>(getRenderer(), jop::LightSource::Type::Directional).setIntensity(jop::Color(1.0f, 0.5f, 0.0f)).setAttenuation(50);
        light->setRotation(-0.1f, 0.0f, 0.f);

        //platform
        //auto platform = createChild("Platform");
        //platform->setPosition(0.0f, -24.0f, 0.0f);
        //platform->setRotation(PI / 2.0f, 0.0f, 0.0f);

        //for (int i = 0; i < 4; i++)
        //{
        //    createRing(platform.get(), 8.0f + 2.0f*i);
        //}
    }

    void preUpdate(const float dt) override
    {
        float time = jop::Engine::getTotalTime() / 2.0f;

        for (int i = 0; i < 6; i++)
        {
            if (i % 2 == 0)
            {
                m_rings[i]->setRotation(time * (1.0f - i / 10.0f), 0.0f, 0.0f);
            }
            else
            {
                m_rings[i]->setRotation(0.0f, time * (1.0f - i / 10.0f), 0.0f);
            }
        }
        m_time += dt;
        auto tList = findChildrenWithTag("target", true);

        if (tList.size() > 0)
        {
            auto t = tList[rand() % tList.size()];
            if (m_time > 0.1f)
            {
                m_time -= 0.1f;
                auto o = createChild("missile");
                o->setPosition(0.0f, 0.0f, 0.0f);

                //glm::vec3 direction = o->getPosition() - t->getGlobalPosition();
                //direction = glm::normalize(direction) * 10.0f;
                //o->createComponent<Bullet>().setVelocity(direction);

                o->createComponent<Missile>().setTarget(t);

                o->createComponent<jop::GenericDrawable>(getRenderer());
                auto drawable = o->getComponent<jop::GenericDrawable>();
                drawable->setModel(jop::Model(jop::Mesh::getDefault(), jop::ResourceManager::getExistingResource<jop::Material>("bulletMaterial")));
            }
        }
    }

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
};

#endif