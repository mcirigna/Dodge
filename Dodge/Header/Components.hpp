#pragma once

#include "../Ecs/Ecs.hpp"
#include <SFML/Graphics.hpp>

// Circle Collider
struct Collider
{
    Collider()
    : position(0,0), radius(0) {}
    
    Collider(sf::Vector2f position, float radius)
    : position(position), radius(radius) {}
    
    sf::Vector2f position;
    unsigned int radius;
    
    bool intersects(const Collider& other)
    {
        float distance = sqrt(pow(abs(position.x-other.position.x), 2) + pow(abs(position.y-other.position.y), 2));
        return distance < (float)(radius + other.radius);
    }
    
    bool touches(const sf::Vector2f point)
    {
        return point.x <= position.x + radius && point.x >= position.x - radius && point.y <= position.y + radius && point.y >= position.y - radius;
    }
};

struct Star : ecs::Component<Star>
{

};

struct Player : ecs::Component<Player>
{
    
};

struct Enemy : ecs::Component<Enemy>
{
    sf::Vector2f bounce;
};

struct RigidBody : ecs::Component<RigidBody>
{
    bool isCollidable           = false;
    bool isMovable              = false;
    
    float mass = 1;
    float radius;
    Collider collider;
    sf::CircleShape shape;
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
};
