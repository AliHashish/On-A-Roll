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

    // for point and spot lights (intesity of light depending on distance)
    struct Attenuation {
        float constant = 1.0f;
        float linear = 0.0f;
        float quadratic = 0.0f;
    };

    // angle for spot lights (similar to kashaf)
    struct Angle {
        float inner = 0.0f;
        float outer = 0.1f;
    };


    class LightComponent : public Component {
    public:
        LightType type = LightType::DIRECTIONAL;        // Default to directional light
    


    // We also define the color & intensity of the light for each component of the Phong model (Ambient, Diffuse, Specular) for more flexbility.
        glm::vec3 diffuse = {0,0,0};
        glm::vec3 specular = {0,0,0};
        glm::vec3 ambient = {0,0,0};
        // glm::vec4 color = {1, 1, 1, 1}; // not used as the above replace color
        Attenuation attenuation; // for spot & point light
        Angle spot_angle; // for spot light

        // The ID of this component type is "Camera"
        static std::string getID() { return "Light"; }

        // Reads light parameters from the given json object
        void deserialize(const nlohmann::json& data) override;
    };

}