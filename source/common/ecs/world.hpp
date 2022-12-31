#pragma once

#include <unordered_set>
#include "../components/light.hpp"
#include "entity.hpp"

namespace our
{
    struct Rectangle {
        // lower left corner coordinates
        float x;
        float y;
        // rectangle width and height
        float width;
        float height;

        Rectangle()
        {
            x = 0.0f;
            y = 0.0f;
            width = 0.0f;
            height = 0.0f;
        }
        Rectangle(float x, float y, float width, float height)
        {
            this->x = x;
            this->y = y;
            this->width = width;
            this->height = height;
        }

        // Check if this coordinate is inside the rectangle
        bool contains(float x, float y)
        {
            // our rectangle looks like this:
            // (x,y-height)     (x+width,y-height)
            // +-----------------+
            // |                 |
            // |                 |
            // |     (>x,<y)     |
            // |                 |
            // |                 |
            // +-----------------+
            // (x,y)            (x+width,y)
            // Notice that y decreases as we move upward (forward in game, as the game is like a top view)
            
            float tolerance = 0.1f;
            return (x >= this->x - tolerance && x <= this->x + this->width + tolerance) && (y <= this->y + 13 * tolerance && y >= this->y - this->height + 6 * tolerance);
            // We add a much greater tolerance for the y coordinate to account for the fact that the origin of 
            // the player is not exactly where the player would expect to find it

            // return (x >= this->x && x <= this->x + this->width) && (y >= this->y && y <= this->y + this->height);
        }

    };

    // This class holds a set of entities
    class World
    {
        std::unordered_set<LightComponent*> lights;
        std::unordered_set<Entity *> entities;         // These are the entities held by this world
        std::unordered_set<Entity *> markedForRemoval; // These are the entities that are awaiting to be deleted
                                                       // when deleteMarkedEntities is called
        std::vector<Rectangle> PlatformRectangles;
        
    public:
        Rectangle WinningRectangle;     // if the player enters this rectangle, they win
        World() = default;

        // This will deserialize a json array of entities and add the new entities to the current world
        // If parent pointer is not null, the new entities will be have their parent set to that given pointer
        // If any of the entities has children, this function will be called recursively for these children
        void deserialize(const nlohmann::json &data, Entity *parent = nullptr);

        // This adds an entity to the entities set and returns a pointer to that entity
        // WARNING The entity is owned by this world so don't use "delete" to delete it, instead, call "markForRemoval"
        // to put it in the "markedForRemoval" set. The elements in the "markedForRemoval" set will be removed and
        // deleted when "deleteMarkedEntities" is called.
        Entity *add()
        {
            // TODO: (Req 8) Create a new entity, set its world member variable to this,    [DONE]
            //  and don't forget to insert it in the suitable container. 
            Entity *newEntity = new Entity();
            newEntity->world = this;
            
            entities.insert(newEntity);
            return newEntity;
        }

        // This returns and immutable reference to the set of all entites in the world.
        const std::unordered_set<Entity *> &getEntities()
        {
            return entities;
        }

        // This marks an entity for removal by adding it to the "markedForRemoval" set.
        // The elements in the "markedForRemoval" set will be removed and deleted when "deleteMarkedEntities" is called.
        void markForRemoval(Entity *entity)
        {
            // TODO: (Req 8) If the entity is in this world, add it to the "markedForRemoval" set.  [DONE]
            if (entities.find(entity) != entities.end())
            {
                markedForRemoval.insert(entity);
            }
        }

        // This removes the elements in "markedForRemoval" from the "entities" set.
        // Then each of these elements are deleted.
        void deleteMarkedEntities()
        {
            // TODO: (Req 8) Remove and delete all the entities that have been marked for removal. [DONE]
            for (auto it = markedForRemoval.begin(); it != markedForRemoval.end(); it++)
            {
                if (entities.find(*it) != entities.end())
                {
                    delete *it;
                    entities.erase(it);
                }
            }
            markedForRemoval.clear();
        }

        // This deletes all entities in the world
        void clear()
        {
            // TODO: (Req 8) Delete all the entites and make sure that the containers are empty     [DONE]
            for(auto entity : entities){
                delete entity;
            }
            //std::cerr << "b3d clear" << std::endl;
            entities.clear();
            markedForRemoval.clear();
        }

        // Checks if these coordinates lie inside any of the rectangles
        bool checkCollision(float x, float y)
        {
            for (auto rectangle : PlatformRectangles)
            {
                if (rectangle.contains(x, y))
                {
                    return true;
                }
            }
            return false;
        }

        // Since the world owns all of its entities, they should be deleted alongside it.
        ~World()
        {
            clear();
        }

        // The world should not be copyable
        World(const World &) = delete;
        World &operator=(World const &) = delete;
    };

}
