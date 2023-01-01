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

        bool winningState = false; // has the player won the game
        // This should be called every frame to update all entities containing a MovementComponent. 
        void update(World* world, float deltaTime) {
            // For each entity in the world
            for(auto entity : world->getEntities()){
                // Check for camera component
                CameraComponent* camera = entity->getComponent<CameraComponent>();
                // Check if the camera component exists
                if(camera){
                    // printing camera rotation
                    // Notice that angles are in radian

                    // printing camera position


                    // deserialize camera to get its components
                    // entity->deserialize();

                    // We will now handle 'physics collisions' through coordinates
                    // std::cout << "Camera position: " << entity->localTransform.position.x << ", " << entity->localTransform.position.y << ", " << entity->localTransform.position.z - 8 << std::endl;
                    // 8albn hnzawed 8 3l z 
                    // hne7tafez bl vertices (lower left corner, width (2*scale), height (2*scale) )
                    // get child component of camera
                    // camera->getOwner()->getChildren();
                    
                    // Check if x,y lie within any of the rectangles of the world
                    // check lw el y < 0, y3ny howa bada2 yo2a3, fa 5leeh ykamel w2oo3, 7ata lw nzl ta7t platform
                    
                    if (!(world->checkCollision(entity->localTransform.position.x - 8 * glm::sin(entity->localTransform.rotation.y), entity->localTransform.position.z - 8 * glm::cos(entity->localTransform.rotation.y))) || entity->localTransform.position.y < -0.4f) {
                        entity->localTransform.position.y -= 0.2f;
                        // notice that for the player, we need to do some coordinate corrections, as we have the coordinates of the camera only
                        // so we deduce the coordinates of the player from the camera coordinates such that
                        
                        // We are going to Draw a square
                        //                             camera rotation 180
                        //                                 (0,-6) 
                        //                           +-----------------+
                        //                           |                 |
                        //                           |                 |
                        //                           |     player      |
                        // camera rotation (-8,2)    |     (0,2)       | (8,2) camera rotation 90
                        //   270                     |                 |
                        //                           |                 |
                        //                           |                 |
                        //                           +-----------------+
                        //                                 (0,10)
                        //                               camera rotation 0

                        // so we just need to remove 8 * sin(rotation) from x
                        // and remove 8 * cos(rotation) from z



                    }
                    
                    // Check if the player has won the game
                    if (world->WinningRectangle.contains(entity->localTransform.position.x - 8 * glm::sin(entity->localTransform.rotation.y), entity->localTransform.position.z - 8 * glm::cos(entity->localTransform.rotation.y))) {
                        // same correction is used here
                        winningState = true;
                    }
                    
                    // check if y is less than -15
                    if (entity->localTransform.position.y < -15.0f) {
                        // return to original coordinares (respawn)
                        entity->localTransform.position.x = 0.0f + 8 * glm::sin(entity->localTransform.rotation.y);
                        entity->localTransform.position.y = 0.0f;
                        entity->localTransform.position.z = 2.0f + 8 * glm::cos(entity->localTransform.rotation.y);
                        // this time, we add the correction instead of removing it, as this is relative to the camera
                        // (k2nena bn3ml el 3aks)
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
