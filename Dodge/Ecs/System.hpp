#pragma once

#include "EntityManager.hpp"

namespace ecs
{
    
    struct System
    {
        virtual void update(EntityManager& em, float dt = 0) = 0;
    };
    
}
