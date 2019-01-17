#include "Component.hpp"

namespace ecs
{
    
    std::unordered_map<IComponent::Type, IComponent::Info> IComponent::m_types;
    
    IComponent::Type IComponent::registerComponentType(CreateFunction createFunction, FreeFunction freeFunction, size_t size)
    {
        IComponent::Info info(createFunction, freeFunction, size);
        IComponent::Type type = (IComponent::Type) m_types.size();
        
        m_types.insert(std::unordered_map<Type, Info>::value_type(type, info));
        
        return type;
    }
    
    IComponent::CreateFunction IComponent::getCreateFunction(IComponent::Type type)
    {
        return std::get<0>(m_types[type]);
    }
    
    IComponent::FreeFunction IComponent::getFreeFunction(IComponent::Type type)
    {
        return std::get<1>(m_types[type]);
    }
    
    size_t IComponent::getSize(IComponent::Type type)
    {
        return std::get<2>(m_types[type]);
    }
    
    bool IComponent::isValidType(IComponent::Type type)
    {
        return m_types.find(type) != m_types.end();
    }
    
}
