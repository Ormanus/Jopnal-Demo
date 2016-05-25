#ifndef MAP_COMPONENT_H
#define MAP_COMPONENT_H

#include <Jopnal/Jopnal.hpp>
#include <Jopnal/Core/Component.hpp>

class MapComponent : public jop::Component
{
public:

	MapComponent(jop::Object& objRef)
		: jop::Component(objRef, 0)
	{
	};

	MapComponent(const MapComponent& misRef, jop::Object& objRef)
		: jop::Component(objRef, 0)
	{
	};

	JOP_GENERIC_COMPONENT_CLONE(MapComponent);

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
		terrain->createComponent<jop::RigidBody>(o->getScene().getWorld(), jop::RigidBody::ConstructInfo(jop::ResourceManager::getNamedResource<jop::TerrainShape>("mesh", points)));
		auto& mesh = jop::ResourceManager::getNamedResource<jop::Mesh>("mesh", vertices, std::vector<unsigned int>());
		auto& drawable1 = terrain->createComponent<jop::GenericDrawable>(o->getScene().getRenderer());
		drawable1.setModel(jop::Model(mesh, material1));
	}
	void generatePath(float scale)
	{
		m_path.clear();

		for (int i = 0; i < m_numWaypoints; i++)
		{
			float x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX)* scale * 32.0f;
			float z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX)* scale * 32.0f;
			float y = terrainY(x, z) + 4.0f;

			m_path.push_back(glm::vec3(x, y, z));
		}
	}
	float terrainY(float x, float z)
	{
		return sin(x * 5.f)*sin(z * 5.f)*10.0f;
	}

	const std::vector<glm::vec3>& getPath()
	{
		return m_path;
	}

private:
	std::vector<glm::vec3> m_path;
	const int m_numWaypoints = 2;
};


#endif
