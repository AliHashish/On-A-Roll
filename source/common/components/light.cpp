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
        // Get color from JSON
        color = data.value("color", color);
        // Get Shininess from JSON
        shininess = data.value("shininess", shininess);





        
        //  return;
        //TODO: (Req 8) Get the material and the mesh from the AssetLoader by their names [DONE]
        // Notice how we just get a string from the json file and pass it to the AssetLoader to get us the actual asset
        // which are defined with the keys "mesh" and "material" in data.
        // Hint: To get a value of type T from a json object "data" where the key corresponding to the value is "key",
        // you can use write: data["key"].get<T>().
        // Look at "source/common/asset-loader.hpp" to know how to use the static class AssetLoader  
        
        // mesh = AssetLoader<Mesh>::get(data["mesh"].get<std::string>());
        // material = AssetLoader<Material>::get(data["material"].get<std::string>());
        
        //std::cerr << "\n----------------------------\n" << mesh << std::endl;
        //std::cerr << "\n----------------------------\n" << material << std::endl;

    }
}