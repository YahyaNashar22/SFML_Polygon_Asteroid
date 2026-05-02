#pragma once

#include <algorithm>
#include <map>
#include <memory>
#include <vector>

#include "Entity.hpp"

using EntityVec = std::vector<std::shared_ptr<Entity>>;

class EntityManager
{
	EntityVec			 m_entities;
	EntityVec			 m_entitiesToAdd;
	std::map<std::string, EntityVec> m_entityMap;
	size_t				 m_totalEntities = 0;

	void removeDeadEntities(EntityVec& vec)
	{
		vec.erase(std::remove_if(vec.begin(), vec.end(),
					 [](const std::shared_ptr<Entity>& e)
					 { return !e->isActive(); }),
			  vec.end());
	}

       public:
	EntityManager() = default;

	void update()
	{
		// Add entities from m_entitiesToAdd to the vector  all entities
		m_entities.insert(m_entities.end(), m_entitiesToAdd.begin(),
				  m_entitiesToAdd.end());

		// Add entities from m_entitiesToAdd to the vector inside the
		// map, with the tag as a key
		for (auto& e : m_entitiesToAdd)
		{
			m_entityMap[e->tag()].push_back(e);
		}

		// Clear added entities so they don't get added in the next
		// iteration
		m_entitiesToAdd.clear();

		// remove dead entities from the vector of all entities
		removeDeadEntities(m_entities);

		// remove dead entities from each vector in the entity map
		// C++20 way of iterating through [key,value] pairs in a map
		for (auto& [tag, entityVec] : m_entityMap)
		{
			removeDeadEntities(entityVec);
		}
	}

	std::shared_ptr<Entity> addEntity(const std::string& tag)
	{
		// create the entity shared pointer
		auto entity =
		    std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));

		// add it to the vec of entitiesToAdd
		m_entitiesToAdd.push_back(entity);

		return entity;
	}

	const EntityVec& getEntities() { return m_entities; }

	const EntityVec& getEntities(const std::string& tag)
	{
		if (m_entityMap.find(tag) == m_entityMap.end())
		{
			m_entityMap[tag] = EntityVec();
		}
		return m_entityMap[tag];
	}

	const std::map<std::string, EntityVec>& getEntityMap()
	{
		return m_entityMap;
	}
};
