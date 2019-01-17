#pragma once

#include "../Ecs/Ecs.hpp"
#include <SFML/Graphics.hpp>
#include "Components.hpp"

const sf::Vector2f gravity(0,250);
const float playerForce = 200;

////////////////////
// Spawner //
////////////////////
struct Spawner : ecs::System
{
    Spawner()
    : m_timer(100) {}
    
    void init(ecs::EntityManager& em)
    {
        float radius = 10;
        
        auto player = em.createEntity();
        em.addComponentToEntity<Player>(player);
        auto rbPlayer = em.addComponentToEntity<RigidBody>(player);
        rbPlayer->isCollidable = true;
        rbPlayer->isMovable = true;
        rbPlayer->radius = radius;
        rbPlayer->position = sf::Vector2f(50,100);
        
//        auto star = em.createEntity();
//        em.addComponentToEntity<Star>(star);
//        auto rbStar = em.addComponentToEntity<RigidBody>(star);
//        rbStar->isCollidable = false;
//        rbStar->isMovable = false;
//        rbStar->radius = radius;
//        rbStar->position = sf::Vector2f(500,10);
//        rbStar->shape.setFillColor(sf::Color::Yellow);
        
    }
    
    void update(ecs::EntityManager& em, float dt)
    {
        m_timer--;
        if (m_timer < 0 && em.getEntitiesWithComponents({Enemy::TYPE}).size() < 10)
        {
            auto enemy = em.createEntity();
            auto en = em.addComponentToEntity<Enemy>(enemy);
            en->bounce = gravity * -2.0f;
            auto rbEnemy = em.addComponentToEntity<RigidBody>(enemy);
            rbEnemy->isCollidable = true;
            rbEnemy->isMovable = true;
            rbEnemy->radius = 10;
            rbEnemy->position = sf::Vector2f(10,10);
            rbEnemy->shape.setFillColor(sf::Color::Red);
            rbEnemy->velocity.x = 500;
            
            m_timer = 100;
        }
    }
    
private:
    float m_timer;
};

///////////
// INPUT //
///////////
struct Input : ecs::System
{
    Input()
    : isPressingUp(false), isPressingDown(false), isPressingLeft(false), isPressingRight(false) {}
    
    void send(sf::Keyboard::Key key)
    {
        switch (key)
        {
            case sf::Keyboard::Up:
                isPressingUp = true;
                break;
            case sf::Keyboard::Down:
                isPressingDown = true;
                break;
            case sf::Keyboard::Left:
                isPressingLeft = true;
                break;
            case sf::Keyboard::Right:
                isPressingRight = true;
                break;
        }
    }
    
    void update(ecs::EntityManager& em, float dt)
    {
        // Forced
        sf::Vector2f left(-playerForce,0);
        sf::Vector2f right(playerForce,0);
        sf::Vector2f jump(0,-2*playerForce);
        sf::Vector2f down(0, playerForce);
        
        // apply to players
        auto eIDs = em.getEntitiesWithComponents({Player::TYPE, RigidBody::TYPE});
        for (auto eID : eIDs)
        {
            auto rb = em.getComponentFromEntity<RigidBody>(eID);
            auto pl = em.getComponentFromEntity<Player>(eID);
            if (rb == nullptr || pl == nullptr) continue;
            
            if (isPressingUp)
            {
                rb->acceleration += jump;
            }
            if (isPressingLeft)
            {
                rb->acceleration += left;
            }
            if (isPressingRight)
            {
                rb->acceleration += right;
            }
            if (isPressingDown)
            {
                rb->acceleration += down;
            }
        }
        
        isPressingUp = false;
        isPressingDown = false;
        isPressingLeft = false;
        isPressingRight = false;
    }
    
private:
    bool isPressingUp;
    bool isPressingDown;
    bool isPressingLeft;
    bool isPressingRight;
};

///////////
// Enemy //
///////////
struct EnemySystem : ecs::System
{
    EnemySystem(float ground)
    : m_ground(ground) {}
    
    void update(ecs::EntityManager& em, float dt)
    {
        auto enemies = em.getEntitiesWithComponents({Enemy::TYPE, RigidBody::TYPE});
        for (auto enemy : enemies)
        {
            auto en = em.getComponentFromEntity<Enemy>(enemy);
            auto rb = em.getComponentFromEntity<RigidBody>(enemy);
            if (en == nullptr || rb == nullptr) continue;
            
            // check collision with ground!
            
            sf::Vector2f velocity = rb->velocity + rb->acceleration * dt;
            sf::Vector2f desiredPosition = rb->position + velocity * dt;
            Collider desiredCollider(desiredPosition, rb->radius);
            
            
            if (desiredCollider.touches(sf::Vector2f(desiredCollider.position.x, m_ground - 1)))
            {
                rb->acceleration = sf::Vector2f(0,0);
                if (rb->velocity.y > 0) rb->velocity.y *= -1.0f;
                //                rb->acceleration.y += -100;
            }
        }
    }
private:
    float m_ground;
};

////////////
// Motion //
////////////
struct Motion : ecs::System
{
    Motion(sf::FloatRect boundary)
    : m_boundary(boundary) {}
    
    void update(ecs::EntityManager& em, float dt)
    {
        //move
        auto rbs = em.getComponentsOfType<RigidBody>();
        for (auto rb : rbs)
        {
            if (!rb->isMovable) continue;
            
            /* calculate movement */
            rb->velocity += rb->acceleration * dt;
            sf::Vector2f desiredPosition = rb->position + rb->velocity * dt;
            Collider desiredCollider(desiredPosition, rb->radius);
            
            rb->acceleration.x = 0;
            rb->acceleration.y = 0;
            
            
            // Check bounds
            float leftSide = desiredCollider.position.x - desiredCollider.radius;
            float rightSide = desiredCollider.position.x + desiredCollider.radius;
            float topSide = desiredCollider.position.y - desiredCollider.radius;
            float bottomSide = desiredCollider.position.y + desiredCollider.radius;
            
            if (leftSide <= m_boundary.left) rb->velocity.x /= -1.5f;
            if (rightSide >= m_boundary.left + m_boundary.width) rb->velocity.x /= -1.5f;
            if (topSide <= m_boundary.top) rb->velocity.y /= -1.5f;
            if (bottomSide >= m_boundary.top + m_boundary.height) rb->velocity.y /= -1.5f;
            
            desiredPosition = rb->position + rb->velocity * dt;
            
            // Check collision
            for (auto other : rbs)
            {
                if (other == rb) continue;
                if (!other->isCollidable) continue;
                
                sf::Vector2f distance(desiredPosition.x-other->position.x, desiredPosition.y-other->position.y);
                float length = sqrt(pow(distance.x, 2) + pow(distance.y, 2));
                if (length > rb->radius + other->radius) continue;
                
                // Collided!
                
                // Unit Vectors
                sf::Vector2f towardsRb = distance / length;
                sf::Vector2f towardsOther = towardsRb * -1.0f;
                
                float rbSpeed = sqrt(pow(rb->velocity.x, 2) + pow(rb->velocity.y, 2));
                float otherSpeed = sqrt(pow(other->velocity.x, 2) + pow(other->velocity.y, 2));
                
                rb->velocity = towardsRb * otherSpeed;
                other->velocity = towardsOther * rbSpeed;
                other->acceleration = sf::Vector2f(0,0);
                
                desiredPosition = rb->position + rb->velocity * dt;
            }
            
            rb->position = desiredPosition;
            rb->acceleration += gravity;
            
        }
    }
private:
    sf::FloatRect m_boundary;
};

////////////
// Render //
////////////
struct Render : ecs::System
{
    Render(sf::RenderWindow* window)
    : m_window(window) {}
    
    void update(ecs::EntityManager& em, float dt = 0)
    {
        auto rbs = em.getComponentsOfType<RigidBody>();
        for (RigidBody* rb : rbs)
        {
            rb->shape.setPosition(rb->position.x - rb->radius, rb->position.y - rb->radius);
            rb->shape.setRadius(rb->radius);
            m_window->draw(rb->shape);
        }
    }
    
private:
    sf::RenderWindow* m_window;
};
