#pragma once

#include "Component.hpp"
#include <unordered_map>
#include <list>

namespace ecs
{
    
    class EntityManager
    {
    public:
        
        //////////////
        /* Entities */
        //////////////
        
        typedef unsigned int EntityID;
        
        EntityID createEntity();
        EntityID createEntity(std::list<IComponent*> components);
        
        template<typename A>
        EntityID createEntity(A& a);
        template<typename A, typename B>
        EntityID createEntity(A& a, B& b);
        template<typename A, typename B, typename C>
        EntityID createEntity(A& a, B& b, C& c);
        template<typename A, typename B, typename C, typename D>
        EntityID createEntity(A& a, B& b, C& c, D& d);
        
        std::list<EntityID> getEntitiesWithComponents(std::list<IComponent::Type> types);
        
        ////////////////
        /* Components */
        ////////////////
        
        IComponent*             addComponentToEntity(IComponent* component, EntityID eID);
        template<typename T>T*  addComponentToEntity(EntityID eID);
        
        
        IComponent*             getComponentFromEntity      (IComponent::Type type, EntityID eID);
        template<typename T>T*  getComponentFromEntity      (EntityID eID);
        
        std::list<IComponent*>  getComponentsFromEntity     (std::list<IComponent::Type> types, EntityID eID);
        std::list<IComponent*>  getComponentsFromEntity     (EntityID eID);
        
        template<typename T>
        std::list<T*>           getComponentsOfType         ();
        std::list<IComponent*>  getComponentsOfType         (IComponent::Type type);
        
        void removeEntity(EntityID eID);
        void removeComponentFromEntity(IComponent::Type type, EntityID eID);
        
    private:
        typedef std::unordered_map<IComponent::Type, IComponent*> ComponentMap;
        typedef std::unordered_map<EntityID, ComponentMap> EntityMap;
        EntityMap m_entities;
    };
    
    
    
    
    
    
    
    
    /* template definitions */
    
    template<typename A>
    EntityManager::EntityID EntityManager::createEntity(A& a)
    {
        std::list<IComponent*> components;
        components.push_back((IComponent*) &a);
        return createEntity(components);
    }
    
    template<typename A, typename B>
    EntityManager::EntityID EntityManager::createEntity(A& a, B& b)
    {
        std::list<IComponent*> components;
        components.push_back((IComponent*) &a);
        components.push_back((IComponent*) &b);
        return createEntity(components);
    }
    
    template<typename A, typename B, typename C>
    EntityManager::EntityID EntityManager::createEntity(A& a, B& b, C& c)
    {
        std::list<IComponent*> components;
        components.push_back((IComponent*) &a);
        components.push_back((IComponent*) &b);
        components.push_back((IComponent*) &c);
        return createEntity(components);
    }
    
    template<typename A, typename B, typename C, typename D>
    EntityManager::EntityID EntityManager::createEntity(A& a, B& b, C& c, D& d)
    {
        std::list<IComponent*> components;
        components.push_back((IComponent*) &a);
        components.push_back((IComponent*) &b);
        components.push_back((IComponent*) &c);
        components.push_back((IComponent*) &d);
        return createEntity(components);
    }
    
    template<typename T>
    T*  EntityManager::addComponentToEntity(EntityID eID)
    {
        if (m_entities.find(eID) != m_entities.end()) // found entity
        {
            IComponent* component;
            
            if (m_entities[eID].find(T::TYPE) != m_entities[eID].end()) // found component
            {
                component = m_entities[eID][T::TYPE];
            }
            else // create component
            {
                IComponent::CreateFunction createFunc = T::CREATE_FUNCTION;
                component = createFunc(nullptr);
                m_entities[eID].insert({component->getType(), component});
            }
            
            return (T*) component;
        }
        
        return nullptr;
    }
    
    template<typename T>
    T*  EntityManager::getComponentFromEntity      (EntityID eID)
    {
        return (T*) getComponentFromEntity(T::TYPE, eID);
    }
    
    template<typename T>
    std::list<T*>  EntityManager::getComponentsOfType         ()
    {
        std::list<T*> components;
        
        for (auto entityMap = m_entities.begin(); entityMap != m_entities.end(); entityMap++)
        {
            if (entityMap->second.find(T::TYPE) != entityMap->second.end())
            {
                components.push_back((T*) entityMap->second[T::TYPE]);
            }
        }
        
        return components;
    }
    
}

