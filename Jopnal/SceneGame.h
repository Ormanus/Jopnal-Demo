#ifndef SCENE_GAME_H
#define SCENE_GAME_H

#define PI 3.14159265359

//#include <Jopnal/Jopnal.hpp>
#include <random>
#include "Tower.h"
#include "HUDComponent.h"
#include "Enemy.h"

enum Action
{
    SELECT,
    BULLET_TOWER,
    MISSILE_TOWER,
};

//----- The Master Component -----
class GameController : public jop::Component
{
public:
    GameController(jop::Object& objRef)
        : jop::Component(objRef, "GC"),
        m_action(SELECT)
    {

    };

    GameController(const GameController& misRef, jop::Object& objRef)
        : jop::Component(objRef, "GC")
    {
    };

    JOP_GENERIC_COMPONENT_CLONE(GameController);



    void mouseLeft()
    {
        switch (m_action)
        {
        case SELECT:
        {
            //try to click buttons

            for (auto object : getObject()->getScene().findChildrenWithTag("HUD", true))
            {
                auto button = object->getComponent<Button>();
                if (button != nullptr)
                {
                    if (button->click())
                    {
                        //"collision" found, don't click other buttons or select towers
                        return;
                    }
                }
            }

            //else
            //TODO: select tower
            break;
        }
        case BULLET_TOWER:
        {
            break;
        }
        case MISSILE_TOWER:
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
            m_action = SELECT;
            break;
        }
        default:
            break;
        }
    }

    void setAction(int a)
    {
        if (m_action == a)
        {
            m_action = SELECT;
        }
        else
        {
            m_action = static_cast<Action>(a);
        }
    }
private:
    Action m_action;
    jop::WeakReference<jop::Object> m_selected;
};

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
            jop::Engine::getCurrentScene().findChild("DEBUG")->getComponent<jop::Text>()->setString("MPos: " + std::to_string(x) + ", " + std::to_string(y));
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
    glm::vec3* m_path;
    const int m_numWaypoints = 16;
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

        generateLevel(&this->getAsObject(), levelScale);
        generatePath(levelScale);

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
        createButton(glm::vec2(420.0f, 200.0f), glm::vec2(-1.0f), "textures/button_bullet.png")->setMessage( "[Co] setAction 1");
        createButton(glm::vec2(420.0f, 000.0f), glm::vec2(-1.0f), "textures/button_missile.png")->setMessage("[Co] setAction 2");;
        createButton(glm::vec2(420.0f, -200.0f), glm::vec2(-1.0f), "textures/button_shield.png")->setMessage("[Co] setAction 0");;

        //degug display
        auto debugObject = createChild("DEBUG");
        debugObject->setScale(800.0f, 800.0f, 1.0f);
        jop::Text& text = debugObject->createComponent<jop::Text>(getRenderer());
        text.setFont(jop::ResourceManager::getResource<jop::Font, std::string, int>(std::string("fonts/novem___.ttf"), 64));
        text.setRenderGroup(1);
        text.setString("Mouse Position: ");
        text.setColor(jop::Color(0.0f, 0.9f, 0.0f, 1.0f));
        
        //jop::Engine::sendMessage("[Co] setAction 2");
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
        if (m_enemyTimer > 1.0f)
        {
            m_enemyTimer -= 1.0f;

            float t = jop::Engine::getTotalTime();

            createEnemy(10.f, 10.f, 0)->getComponent<Enemy>()->setPath(m_path, m_numWaypoints);
        }
    }
private:
    void generateLevel(jop::Object* o, float scale)
    {
        //const float scale = 10.0f;

        auto terrain = o->createChild("terrain");
        auto& material1 = jop::ResourceManager::getEmptyResource<jop::Material>("terrainMaterial", jop::Material::Attribute::Default);
        material1.setMap(jop::Material::Map::Diffuse, jop::ResourceManager::getResource<jop::Texture2D>("textures/Terrain.png", true));

        std::vector<jop::Vertex> vertices;

        const int w = 32;
        const int h = 32;

        std::vector<glm::vec3> points;

        for (int i = 0; i < w; i++)
        {
            for (int j = 0; j < h; j++)
            {
                //create mesh
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
    void generatePath(float scale)
    {
        m_path = new glm::vec3[m_numWaypoints];

        for (int i = 0; i < m_numWaypoints; i++)
        {
            float x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * scale * 32.0f;
            float z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * scale * 32.0f;
            float y = terrainY(x, z) + 4.0f;

            m_path[i] = glm::vec3(x, y, z);
        }
    }
    float terrainY(float x, float z)
    {
        return sin(x * 5.f)*sin(z * 5.f)*10.0f;
    }
    jop::WeakReference<jop::Object> createEnemy(float x, float y, int type)
    {
        //TODO: switch(type) & different enemy types
        auto o = createChild("enemy");
        o->createComponent<Enemy>();
        return o;
    }

    Button* createButton(glm::vec2 position, glm::vec2 size, std::string path)
    {
        auto o = findChild("orthoCam")->createChild("button");
        o->createComponent<Button>(path);
        o->setPosition(glm::vec3(position.x, position.y, 1.0f)).setScale(glm::vec3(size.x, size.y, 1.0f));
        return o->getComponent<Button>();
    }
};

#endif