//nothing lolol!

#include "SceneStart.h"
#include "SceneGame.h"

SceneStart::SceneStart()
	: jop::Scene("Start")
{
	jop::Engine::getSubsystem<jop::Window>()->setMouseMode(jop::Mouse::Mode::Visible);

	getRenderer().setMask(3);

	jop::Window* w = jop::Engine::getSubsystem<jop::Window>();
	jop::Engine::getSubsystem<jop::Window>()->setEventHandler<MenuEventHandler>(*w);

	w->setClearColor(jop::Color::Black);

	//GC
	createChild("GC")->createComponent<MenuController>();

	//camera
	//this to make a cool background?
	auto cameraObject = createChild("cam");
	cameraObject->setPosition(0.0f, 0.0f, 0.0f);
	auto cam = cameraObject->createChild("view");
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
	auto textObject = orthoCam->createChild("Text");
	textObject->setPosition(0.f, -100.f, 10.f);
	textObject->setScale(8000.0f, 8000.0f, 1.0f);
	jop::Text& text = textObject->createComponent<jop::Text>(getRenderer());
	text.setFont(jop::ResourceManager::getResource<jop::Font, std::string, int>(std::string("fonts/comic.ttf"), 64));

	text.setString("Main Menu");
	text.setColor(jop::Color(0.8f, 0.7f, 1.0f, 1.50f));
	//text.setRenderGroup(0);

	//cube material
	auto& material = jop::ResourceManager::getEmptyResource<jop::Material>("cubeMaterial", jop::Material::Attribute::DefaultLighting);
	material.setMap(jop::Material::Map::Diffuse, jop::ResourceManager::getResource<jop::Texture2D>("textures/Pixel.png", true));

	//cube
	auto o = createChild("cube");
	auto& drawable = o->createComponent<jop::GenericDrawable>(getRenderer());
	drawable.setModel(jop::Model(jop::Mesh::getDefault(), jop::ResourceManager::getExistingResource<jop::Material>("cubeMaterial")));

	//light
	auto light = createChild("light");
	light->createComponent<jop::LightSource>(getRenderer(), jop::LightSource::Type::Spot).setIntensity(jop::Color(1.0f, 1.0f, 1.0f)).setAttenuation(500);
	light->setRotation(0.f, 0.0f, 0.f);
	light->setPosition(0.f, 20.f, 0.f);

	createButton(glm::vec2(0.0f, 000.0f), glm::vec2(-4.0f, -1.f), "Play", "textures/button_play_0.png", "textures/button_play_2.png", "textures/button_play_1.png")->setMessage(0, "0");
	createButton(glm::vec2(0.0f, -128.0f), glm::vec2(-4.0f, -1.f), "Quit", "textures/button_play_0.png", "textures/button_play_2.png", "textures/button_play_1.png")->setMessage(1, "1");

	getRenderer().setMask(3);
}

void SceneStart::postUpdate(const float dt)
{
	float time = jop::Engine::getTotalTime() / 2.0f;
	m_time += dt;

	if (time > 5.f)
	{
		//
	}
}