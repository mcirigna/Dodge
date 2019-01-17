#pragma once

#include <unordered_map>
#include <tuple>

namespace ecs
{
    
    /* Interface Component */
    struct IComponent
    {
        typedef unsigned int            Type;
        typedef IComponent*             (*CreateFunction)(IComponent* copy);
        typedef void                    (*FreeFunction)(IComponent* toBeFreed);
        
        /* static accessors */
        static Type                     registerComponentType (CreateFunction, FreeFunction, size_t);
        static CreateFunction           getCreateFunction     (Type type);
        static FreeFunction             getFreeFunction       (Type type);
        static size_t                   getSize               (Type type);
        static bool                     isValidType           (Type type);
        
        /* instance accessors */
        virtual CreateFunction          getCreateFunction   () = 0;
        virtual FreeFunction            getFreeFunction     () = 0;
        virtual Type                    getType             () = 0;
        virtual size_t                  getSize             () = 0;
        
    private:
        typedef std::tuple<CreateFunction, FreeFunction, size_t>    Info;
        static std::unordered_map<Type, Info>                       m_types;
    };
    
    /* inherit from this class */
    template<typename T>
    struct Component : public IComponent
    {
        /* static accessors */
        static const CreateFunction     CREATE_FUNCTION;
        static const FreeFunction       FREE_FUNCTION;
        static const Type               TYPE;
        static const size_t             SIZE;
        
        /* instance accessors */
        IComponent::CreateFunction      getCreateFunction() { return CREATE_FUNCTION; }
        IComponent::FreeFunction        getFreeFunction()   { return FREE_FUNCTION;   }
        IComponent::Type                getType()           { return TYPE;            }
        size_t                          getSize()           { return SIZE;            }
    };
    
    /* helper init functions */
    
    template<typename T>
    IComponent* IComponentCreateFunction(IComponent* copy)
    {
        T* t;
        if (copy == nullptr)
        {
            t = new T;
        }
        else
        {
            t = new T(*(T*) copy);
        }
        return (IComponent*) t;
    }
    
    template<typename T>
    void IComponentFreeFunction(IComponent* toBeFreed)
    {
        T* t = (T*) toBeFreed;
        delete t;
    }
    
    /* member init */
    
    template<typename T>
    const IComponent::Type
    Component<T>::TYPE(IComponent::registerComponentType(IComponentCreateFunction<T>, IComponentFreeFunction<T>, sizeof(T)));
    
    template<typename T>
    const IComponent::CreateFunction
    Component<T>::CREATE_FUNCTION(IComponentCreateFunction<T>);
    
    template<typename T>
    const IComponent::FreeFunction
    Component<T>::FREE_FUNCTION(IComponentFreeFunction<T>);
    
    template<typename T>
    const size_t
    Component<T>::SIZE(sizeof(T));
    
    /* helper */
    
    template<typename T>
    T* downcastTo(IComponent* component)
    {
        if (T::TYPE != component->getType()) return nullptr;
        T* t = (T*) component;
        return t;
    }
    
    /* example usage */
    
    /*
     
     
     
     
     
     
     
     */
    
}
