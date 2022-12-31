#pragma once

#include "../ecs/world.hpp"
#include "../components/camera.hpp"
#include "../components/free-camera-controller.hpp"
#include "../components/movement.hpp"

#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

namespace our
{

    // The free camera controller system is responsible for moving every entity which contains a FreeCameraControllerComponent.
    // This system is added as a slightly complex example for how use the ECS framework to implement logic.
    // For more information, see "common/components/free-camera-controller.hpp"
    class FreeCameraControllerSystem
    {
        Application *app;          // The application in which the state runs
        bool mouse_locked = false; // Is the mouse locked

        bool clickedShift = false;  // used in angular velocity
        float forwardShiftMultiplier = 1;  // used in angular velocity
        float sidewaysShiftMultiplier = 1; // used in angular velocity

    public:
        // When a state enters, it should call this function and give it the pointer to the application
        void enter(Application *app)
        {
            this->app = app;
        }

        // This should be called every frame to update all entities containing a FreeCameraControllerComponent
        void update(World *world, float deltaTime)
        {
            // First of all, we search for an entity containing both a CameraComponent and a FreeCameraControllerComponent
            // As soon as we find one, we break
            CameraComponent *camera = nullptr;
            FreeCameraControllerComponent *controller = nullptr;
            MovementComponent *player = nullptr; // points to the player
            

            for (auto entity : world->getEntities())
            {
                if (!(camera && controller))
                {
                    camera = entity->getComponent<CameraComponent>();
                    controller = entity->getComponent<FreeCameraControllerComponent>();
                }
                if (entity->parent)
                {
                    auto CheckParent = entity->parent->getComponent<CameraComponent>();

                    if (CheckParent)
                    {
                        player = entity->getComponent<MovementComponent>();
                    }
                }
                if (camera && controller && player)
                    break;
            }
            // If there is no entity with both a CameraComponent and a FreeCameraControllerComponent, we can do nothing so we return
            if (!(camera && controller))
                return;
            // Get the entity that we found via getOwner of camera (we could use controller->getOwner())
            Entity *entity = camera->getOwner();

            // If the left mouse button is pressed, we lock and hide the mouse. This common in First Person Games.
            if (app->getMouse().isPressed(GLFW_MOUSE_BUTTON_1) && !mouse_locked)
            {
                app->getMouse().lockMouse(app->getWindow());
                mouse_locked = true;
                // If the left mouse button is released, we unlock and unhide the mouse.
            }
            else if (!app->getMouse().isPressed(GLFW_MOUSE_BUTTON_1) && mouse_locked)
            {
                app->getMouse().unlockMouse(app->getWindow());
                mouse_locked = false;
            }

            // We get a reference to the entity's position and rotation
            glm::vec3 &position = entity->localTransform.position;
            glm::vec3 &rotation = entity->localTransform.rotation;

            // If the left mouse button is pressed, we get the change in the mouse location
            // and use it to update the camera rotation
            if (app->getMouse().isPressed(GLFW_MOUSE_BUTTON_1))
            {
                glm::vec2 delta = app->getMouse().getMouseDelta();
                rotation.x -= delta.y * controller->rotationSensitivity; // The y-axis controls the pitch
                rotation.y -= delta.x * controller->rotationSensitivity; // The x-axis controls the yaw
            }

            // We prevent the pitch from exceeding a certain angle from the XZ plane to prevent gimbal locks
            if (rotation.x < -glm::half_pi<float>() * 0.99f)
                rotation.x = -glm::half_pi<float>() * 0.99f;
            if (rotation.x > glm::half_pi<float>() * 0.99f)
                rotation.x = glm::half_pi<float>() * 0.99f;
            // This is not necessary, but whenever the rotation goes outside the 0 to 2*PI range, we wrap it back inside.
            // This could prevent floating point error if the player rotates in single direction for an extremely long time.
            rotation.y = glm::wrapAngle(rotation.y);

            // We update the camera fov based on the mouse wheel scrolling amount
            float fov = camera->fovY + app->getMouse().getScrollOffset().y * controller->fovSensitivity;
            fov = glm::clamp(fov, glm::pi<float>() * 0.01f, glm::pi<float>() * 0.99f); // We keep the fov in the range 0.01*PI to 0.99*PI
            camera->fovY = fov;

            // We get the camera model matrix (relative to its parent) to compute the front, up and right directions
            glm::mat4 matrix = entity->localTransform.toMat4();

            glm::vec3 front = glm::vec3(matrix * glm::vec4(0, 0, -1, 0)),
                      up = glm::vec3(matrix * glm::vec4(0, 1, 0, 0)),
                      right = glm::vec3(matrix * glm::vec4(1, 0, 0, 0));

            glm::vec3 current_sensitivity = controller->positionSensitivity;
            // If the LEFT SHIFT key is pressed, we multiply the position sensitivity by the speed up factor
            if (app->getKeyboard().isPressed(GLFW_KEY_LEFT_SHIFT))
            {
                
                current_sensitivity *= controller->speedupFactor;
                if(!clickedShift)
                {
                    clickedShift = true;
                    forwardShiftMultiplier = controller->speedupFactor / 2.0f;
                    sidewaysShiftMultiplier = controller->speedupFactor / 4.0f;
                }
            }

            if (app->getKeyboard().justReleased(GLFW_KEY_LEFT_SHIFT))
            {
                clickedShift = false;
                forwardShiftMultiplier = 1.0f;
                sidewaysShiftMultiplier = 1.0f;
            }


            // We change the camera position based on the keys WASD/QE
            // S & W moves the player back and forth
            if (app->getKeyboard().isPressed(GLFW_KEY_W) || app->getKeyboard().isPressed(GLFW_KEY_UP))
            {
                position += front * (deltaTime * current_sensitivity.z);
                // update player angular velocity
                player->angularVelocity.x = -5.0f * forwardShiftMultiplier;
            }
            if (app->getKeyboard().isPressed(GLFW_KEY_S) || app->getKeyboard().isPressed(GLFW_KEY_DOWN))
            {
                position -= front * (deltaTime * current_sensitivity.z);
                // update player angular velocity
                player->angularVelocity.x = 5.0f * forwardShiftMultiplier;
            }    
            if ((app->getKeyboard().isPressed(GLFW_KEY_W) || app->getKeyboard().isPressed(GLFW_KEY_UP)) && (app->getKeyboard().isPressed(GLFW_KEY_S) || app->getKeyboard().isPressed(GLFW_KEY_DOWN)))
            {
                player->angularVelocity.x = 0.0f;
            }

            if (app->getKeyboard().justReleased(GLFW_KEY_W) || app->getKeyboard().justReleased(GLFW_KEY_UP))
            {
                player->angularVelocity.x = 0.0f;
            }
            if (app->getKeyboard().justReleased(GLFW_KEY_S) || app->getKeyboard().justReleased(GLFW_KEY_DOWN))
            {
                player->angularVelocity.x = 0.0f;
            }

            // Q & E moves the player up and down
            // if(app->getKeyboard().isPressed(GLFW_KEY_Q)) position += up * (deltaTime * current_sensitivity.y);
            // if(app->getKeyboard().isPressed(GLFW_KEY_E)) position -= up * (deltaTime * current_sensitivity.y);

            // Q & E rotates the camera 90 degress left and right
            // no need to multiply by deltaTime, as we want the rotation to be instantaneous
            // justPressed dyh btol2ot dosa wa7da bs, y3ny lw fedel 3amel hold msh ht3ml 7aga
            // isPressed, btefdal sh8ala tool ma howa dayes (w7na msh 3ayzeen dh hena)
            // glm::smoothstep(0.00f, 2.00f, time);
            if (app->getKeyboard().justPressed(GLFW_KEY_Q))
            {
                rotation.y += glm::half_pi<float>(); // * deltaTime;
                // erg3 wara 8, w emshy ymeen 8
                position.x += 8.0f;
                position.z += 8.0f;
                // print sine and cosine of the rotation and the rotation angle
                std::cout << "sin: " << glm::sin(rotation.y) << " cos: " << glm::cos(rotation.y) << " angle: " << rotation.y << std::endl;
            }
            if (app->getKeyboard().justPressed(GLFW_KEY_E))
            {
                rotation.y -= glm::half_pi<float>(); // * deltaTime;
                // erg3 wara 8, w emshy shmal 8
                position.x -= 8.0f;
                position.z -= 8.0f;
                std::cout << "sin: " << glm::sin(rotation.y) << " cos: " << glm::cos(rotation.y) << std::endl;
            }
            // This could prevent floating point error if the player rotates in single direction for an extremely long time.
            // rotation.y = glm::wrapAngle(rotation.y);

            // if(app->getKeyboard().isPressed(GLFW_KEY_Q)) position += up * (deltaTime * current_sensitivity.y);
            // if(app->getKeyboard().isPressed(GLFW_KEY_E)) position -= up * (deltaTime * current_sensitivity.y);
            // A & D moves the player left or right
            if (app->getKeyboard().isPressed(GLFW_KEY_D) || app->getKeyboard().isPressed(GLFW_KEY_RIGHT))
            {
                position += right * (deltaTime * current_sensitivity.x);
                // update player angular velocity
                player->angularVelocity.z = -5.0f * sidewaysShiftMultiplier;
            }
            if (app->getKeyboard().isPressed(GLFW_KEY_A) || app->getKeyboard().isPressed(GLFW_KEY_LEFT))
            {
                position -= right * (deltaTime * current_sensitivity.x);
                // update player angular velocity
                player->angularVelocity.z = 5.0f * sidewaysShiftMultiplier;
            }

            if ((app->getKeyboard().isPressed(GLFW_KEY_D) || app->getKeyboard().isPressed(GLFW_KEY_RIGHT)) && (app->getKeyboard().isPressed(GLFW_KEY_A) || app->getKeyboard().isPressed(GLFW_KEY_LEFT)))
            {
                player->angularVelocity.z = 0.0f;
            }

            if (app->getKeyboard().justReleased(GLFW_KEY_D) || app->getKeyboard().justReleased(GLFW_KEY_RIGHT))
            {
                player->angularVelocity.z = 0.0f;
            }
            if (app->getKeyboard().justReleased(GLFW_KEY_A) || app->getKeyboard().justReleased(GLFW_KEY_LEFT))
            {
                player->angularVelocity.z = 0.0f;
            }
            
        }

        // When the state exits, it should call this function to ensure the mouse is unlocked
        void exit()
        {
            if (mouse_locked)
            {
                mouse_locked = false;
                app->getMouse().unlockMouse(app->getWindow());
            }
        }
    };

}
