#include "light.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"
#include <iostream>

namespace our {
    // Receives the mesh & material from the AssetLoader by the names given in the json object
    void LightComponent::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;
        // Get diffuse from JSON
        diffuse = data.value("diffuse", diffuse);
        // Get specular from JSON
        specular = data.value("specular", specular);
        // Get ambient from JSON
        ambient = data.value("ambient", ambient);
        // // Get color from JSON
        // color = data.value("color", color);

        if(data.value("lightType", "directional") == "directional"){
            type = our::LightType::DIRECTIONAL;
        }else if(data.value("lightType", "directional") == "point"){
            type = our::LightType::POINT;
            attenuation.constant = data.value("attenuation_constant", attenuation.constant);
            attenuation.linear = data.value("attenuation_linear", attenuation.linear);
            attenuation.quadratic = data.value("attenuation_quadratic", attenuation.quadratic);
        }else if(data.value("lightType", "directional") == "spotLight"){
            type = our::LightType::SPOT;
            attenuation.constant = data.value("attenuation_constant", attenuation.constant);
            attenuation.linear = data.value("attenuation_linear", attenuation.linear);
            attenuation.quadratic = data.value("attenuation_quadratic", attenuation.quadratic);
            spot_angle.inner = data.value("spot_angle_inner", spot_angle.inner);
            spot_angle.outer = data.value("spot_angle_outer", spot_angle.outer);
        }

    }
}