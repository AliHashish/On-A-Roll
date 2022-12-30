#pragma once

#include "../ecs/world.hpp"
#include "../components/movement.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

#include <iostream>

namespace our
{

    // The movement system is responsible for moving every entity which contains a MovementComponent.
    // This system is added as a simple example for how use the ECS framework to implement logic. 
    // For more information, see "common/components/movement.hpp"
    class MovementSystem {
    public:

        // This should be called every frame to update all entities containing a MovementComponent. 
        void update(World* world, float deltaTime) {
            // For each entity in the world
            for(auto entity : world->getEntities()){
                // Check for camera component
                CameraComponent* camera = entity->getComponent<CameraComponent>();
                // Check if the camera component exists
                if(camera){
                    // We will now handle 'physics collisions' through coordinates
                    std::cout << "Camera position: " << entity->localTransform.position.x << ", " << entity->localTransform.position.y << ", " << entity->localTransform.position.z<< std::endl;
                    
                    // hne7tafez bl vertices (left corner, width (2*scale), height (2*scale) )
                    
                    // check lw el y < 0, y3ny howa bada2 yo2a3, fa 5leeh ykamel w2oo3, 7ata lw nzl ta7t platform
                    // if (entity->localTransform.position.x < -10.0f || entity->localTransform.position.y < 0.0f) {
                    //     entity->localTransform.position.y -= 0.2f;
                    // }
                    
                    // check if y is less than -15
                    if (entity->localTransform.position.y < -15.0f) {
                        // return to original coordinares (respawn)
                        entity->localTransform.position.x = 0.0f;
                        entity->localTransform.position.y = 0.0f;
                        entity->localTransform.position.z = 10.0f;
                    }
                }
                // Get the movement component if it exists
                MovementComponent* movement = entity->getComponent<MovementComponent>();
                // If the movement component exists
                if(movement){
                    // Change the position and rotation based on the linear & angular velocity and delta time.
                    entity->localTransform.position += deltaTime * movement->linearVelocity;
                    entity->localTransform.rotation += deltaTime * movement->angularVelocity;
                }
            }
        }

    };

}
