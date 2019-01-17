#include "EntityManager.hpp"

namespace ecs
{
    
    //////////////
    /* Entities */
    //////////////
    
    static unsigned int entityIndex = 0;
    
    EntityManager::EntityID EntityManager::createEntity()
    {
        EntityID eID = (EntityID) entityIndex++;
        m_entities.insert({eID, ComponentMap()});
        return eID;
    }
    
    EntityManager::EntityID EntityManager::createEntity(std::list<IComponent*> components)
    {
        EntityID eID = (EntityID) m_entities.size();
        m_entities.insert({eID, ComponentMap()});
        
        for (auto copy = components.begin(); copy != components.end(); copy++)
        {
            IComponent::CreateFunction createFunc = (*copy)->getCreateFunction();
            IComponent* component = createFunc(*copy);
            m_entities[eID].insert({component->getType(), component});
        }
        
        return eID;
    }
    
    std::list<EntityManager::EntityID> EntityManager::getEntitiesWithComponents(std::list<IComponent::Type> types)
    {
        std::list<EntityManager::EntityID> eIDs;
        
        for (auto entityMap = m_entities.begin(); entityMap != m_entities.end(); entityMap++)
        {
            bool valid = true;
            for (auto type = types.begin(); type != types.end(); type++)
            {
                if (entityMap->second.find(*type) == entityMap->second.end()) valid = false;
            }
            if (valid) eIDs.push_back(entityMap->first);
        }
        
        return eIDs;
    }
    
    ////////////////
    /* Components */
    ////////////////
    
    IComponent* EntityManager::addComponentToEntity(IComponent* copy, EntityID eID)
    {
        IComponent::CreateFunction createFunc = copy->getCreateFunction();
        IComponent* component = createFunc(copy);
        
        if (m_entities.find(eID) != m_entities.end())
        {
            m_entities[eID].insert({component->getType(), component});
        }
        
        return component;
    }
    
    IComponent* EntityManager::getComponentFromEntity(IComponent::Type type, EntityID eID)
    {
        if (m_entities.find(eID) == m_entities.end()) return nullptr;
        if (m_entities[eID].find(type) == m_entities[eID].end()) return nullptr;
        
        return m_entities[eID][type];
    }
    
    std::list<IComponent*>  EntityManager::getComponentsFromEntity     (EntityID eID)
    {
        std::list<IComponent*> components;
        
        if (m_entities.find(eID) == m_entities.end()) return components;
        
        for (auto componentMap = m_entities[eID].begin(); componentMap != m_entities[eID].end(); componentMap++)
        {
            components.push_back(componentMap->second);
        }
        
        return components;
    }
    
    std::list<IComponent*>  EntityManager::getComponentsFromEntity     (std::list<IComponent::Type> types, EntityID eID)
    {
        std::list<IComponent*> components;
        
        if (m_entities.find(eID) == m_entities.end()) return components;
        
        for (auto componentMap = m_entities[eID].begin(); componentMap != m_entities[eID].end(); componentMap++)
        {
            if (std::find(types.begin(), types.end(), componentMap->first) != types.end())
            {
                components.push_back(componentMap->second);
            }
        }
        
        return components;
    }
    
    std::list<IComponent*> EntityManager::getComponentsOfType(IComponent::Type type)
    {
        std::list<IComponent*> components;
        
        for (auto entityMap = m_entities.begin(); entityMap != m_entities.end(); entityMap++)
        {
            if (entityMap->second.find(type) != entityMap->second.end())
            {
                components.push_back(entityMap->second[type]);
            }
        }
        
        return components;
    }
    
    void EntityManager::removeEntity(EntityID eID)
    {
        // find entity
        auto entityMap = m_entities.find(eID);
        if (entityMap == m_entities.end()) return;
        
        // get components
        auto componentMap = m_entities[eID];
        
        // free each component
        for (auto cID : componentMap)
        {
            auto freeFunc = cID.second->getFreeFunction();
            freeFunc(cID.second);
        }
        
        // erase entity
        m_entities.erase(eID);
    }
    
    void EntityManager::removeComponentFromEntity(IComponent::Type type, EntityID eID)
    {
        if (m_entities.find(eID) != m_entities.end())
        {
            m_entities[eID].erase(type);
        }
    }
    
}


