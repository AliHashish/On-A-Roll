#include "material.hpp"

#include "../asset-loader.hpp"
#include "deserialize-utils.hpp"

namespace our
{

    // This function should setup the pipeline state and set the shader to be used
    void Material::setup() const
    {
        // TODO: (Req 7) Write this function [DONE]
        pipelineState.setup(); // Calling the pipeline setup
        shader->use();         // Setting which shader to use
    }

    // This function read the material data from a json object
    void Material::deserialize(const nlohmann::json &data)
    {
        if (!data.is_object())
            return;

        if (data.contains("pipelineState"))
        {
            pipelineState.deserialize(data["pipelineState"]);
        }
        shader = AssetLoader<ShaderProgram>::get(data["shader"].get<std::string>());
        transparent = data.value("transparent", false);

        // if (data.contains("light"))       // deserializing the light object if it exists
        // {
        //     light->deserialize(data["light"]);
        // }
        
    }

    // This function should call the setup of its parent and
    // set the "tint" uniform to the value in the member variable tint
    void TintedMaterial::setup() const
    {
        // TODO: (Req 7) Write this function [DONE]
        Material::setup();         // Calling material setup
        shader->set("tint", tint); // setting the uniform value of tint
    }

    // This function read the material data from a json object
    void TintedMaterial::deserialize(const nlohmann::json &data)
    {
        Material::deserialize(data);
        if (!data.is_object())
            return;
        tint = data.value("tint", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    // This function should call the setup of its parent and
    // set the "alphaThreshold" uniform to the value in the member variable alphaThreshold
    // Then it should bind the texture and sampler to a texture unit and send the unit number to the uniform variable "tex"
    void TexturedMaterial::setup() const
    {
        // TODO: (Req 7) Write this function [DONE]
        TintedMaterial::setup();                       // Calling the tinted material setup
        shader->set("alphaThreshold", alphaThreshold); // setting the alphathreshold uniform value
        if (texture != nullptr)
            texture->bind(); // binding the texture
        if (sampler != nullptr)
            sampler->bind(0);
        shader->set("tex", 0); // setting the tex uniform value
    }

    // This function read the material data from a json object
    void TexturedMaterial::deserialize(const nlohmann::json &data)
    {
        TintedMaterial::deserialize(data);
        if (!data.is_object())
            return;
        alphaThreshold = data.value("alphaThreshold", 0.0f);
        texture = AssetLoader<Texture2D>::get(data.value("texture", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }

    void LitMaterial::setup() const
    {
        TexturedMaterial::setup();
        shader->set("alphaThreshold", alphaThreshold);
        
    }

    //This function read the LitMaterial data from a json object
    void LitMaterial::deserialize(const nlohmann::json &data)
    {
        Material::deserialize(data);
        if (!data.is_object())
            return;
        alphaThreshold = data.value("alphaThreshold", 0.0f);
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
        albedoMap = AssetLoader<Texture2D>::get(data.value("albedoMap", ""));
        specularMap = AssetLoader<Texture2D>::get(data.value("specularMap", ""));
        roughnessMap = AssetLoader<Texture2D>::get(data.value("roughnessMap", ""));
        emissiveMap = AssetLoader<Texture2D>::get(data.value("emissiveMap", ""));
        aoMap = AssetLoader<Texture2D>::get(data.value("aoMap", ""));
    }

}