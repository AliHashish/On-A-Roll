#pragma once

#include "../ecs/component.hpp"
#include "../mesh/mesh.hpp"
// #include "../material/material.hpp"
#include "../asset-loader.hpp"


#include <glm/mat4x4.hpp>

namespace our {

    // We will support 3 types of lights.
    // 1- Directional Light: where we assume that the light rays are parallel. We use this to approximate sun light.
    // 2- Point Light: where we assume that the light source is a single point that emits light in every direction. It can be used to approximate light bulbs.
    // 3- Spot Light: where we assume that the light source is a single point that emits light in the direction of a cone. It can be used to approximate torches, highway light poles.
    enum class LightType {
        DIRECTIONAL,
        POINT,
        SPOT
    };
    // This component denotes that any renderer should draw the scene relative to this camera.
    // We do not define the eye, center or up here since they can be extracted from the entity local to world matrix
    class LightComponent : public Component {
    public:
        LightType type = LightType::DIRECTIONAL;        // Default to directional light
        // LightType type;
    // We also define the color & intensity of the light for each component of the Phong model (Ambient, Diffuse, Specular).
        glm::vec3 diffuse = {0,0,0};
        glm::vec3 specular = {0,0,0};
        glm::vec3 ambient = {0,0,0};
        glm::vec4 color = {1, 1, 1, 1};

        float shininess = 1.0f;

        // The ID of this component type is "Camera"
        static std::string getID() { return "Light"; }

        // Reads camera parameters from the given json object
        void deserialize(const nlohmann::json& data) override;

        // Creates and returns the camera view matrix
        // glm::mat4 getViewMatrix() const;
        
        // Creates and returns the camera projection matrix
        // "viewportSize" is used to compute the aspect ratio
        // glm::mat4 getProjectionMatrix(glm::ivec2 viewportSize) const;
    };

}